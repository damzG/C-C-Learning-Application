// Level map: a winding path of lesson nodes, in the style of a course map.
#include "screens.h"
#include "theme.h"
#include "ui.h"
#include "levels.h"
#include "questions.h"

#include <math.h>
#include <stdio.h>

#define MAP_W      880
#define HUD_H      80
#define NODE_R     34
#define MAX_NODES  64
#define DEV_TOOLS  1   // set to 0 to hide the test buttons

static float nodeX[MAX_NODES];
static float nodeY[MAX_NODES];   // world position, before scrolling
static float worldH;
static float scroll;
static float targetScroll;
static int selected;

static Color SectionColor(int section)
{
    static const Color colors[] = {
        { 72, 202, 228, 255 },   // cyan
        { 255, 140, 66, 255 },   // orange
        { 170, 120, 255, 255 },  // purple
        { 255, 110, 160, 255 },  // pink
        { 255, 200, 60, 255 }    // gold
    };
    return colors[section % 5];
}

static float MaxScroll(void)
{
    float m = worldH - (SCREEN_H - HUD_H);
    return m > 0 ? m : 0;
}

static float ClampScroll(float v)
{
    if (v < 0) v = 0;
    if (v > MaxScroll()) v = MaxScroll();
    return v;
}

static float ScreenY(int i)
{
    return nodeY[i] - scroll + HUD_H;
}

static void CenterOn(int i)
{
    targetScroll = ClampScroll(nodeY[i] - (SCREEN_H - HUD_H) / 2.0f);
}

void ScreenMapEnter(AppState *app)
{
    int n = LevelCount();
    if (n > MAX_NODES) n = MAX_NODES;

    float y = 120.0f;
    int prevSection = -1;
    for (int i = 0; i < n; i++)
    {
        const Level *level = LevelGet(i);
        if (level->section != prevSection)
        {
            y += 60.0f;   // room for the section banner
            prevSection = level->section;
        }
        nodeX[i] = MAP_W / 2.0f + sinf(i * 1.1f) * 170.0f;
        nodeY[i] = y;
        y += 130.0f;
    }
    worldH = y + 40.0f;

    Account *acc = AppCurrentAccount(app);
    int done = acc ? acc->levelsCompleted : 0;
    selected = done < n ? done : n - 1;
    CenterOn(selected);
    scroll = targetScroll;
}

static void DrawNode(int i, int done, bool isSelected, float t)
{
    float x = nodeX[i];
    float y = ScreenY(i);
    Vector2 c = { x, y };

    if (i < done)
    {
        DrawCircleV(c, NODE_R, COL_DONE);
        DrawLineEx((Vector2){ x - 13, y }, (Vector2){ x - 4, y + 10 }, 5.0f, RAYWHITE);
        DrawLineEx((Vector2){ x - 4, y + 10 }, (Vector2){ x + 14, y - 10 }, 5.0f, RAYWHITE);
    }
    else if (i == done)
    {
        float pulse = 6.0f + sinf(t * 4.0f) * 3.0f;
        DrawRing(c, NODE_R + pulse, NODE_R + pulse + 3.0f, 0, 360, 48, Fade(COL_ACCENT, 0.5f));
        DrawCircleV(c, NODE_R, COL_ACCENT);

        char num[8];
        snprintf(num, sizeof(num), "%d", i + 1);
        int w = UiMeasureText(num, 30);
        UiDrawText(num, (int)x - w / 2, (int)y - 15, 30, COL_BG);
    }
    else
    {
        DrawCircleV(c, NODE_R, COL_LOCKED);
        // Padlock
        DrawRing((Vector2){ x, y - 3 }, 6.0f, 9.0f, 180, 360, 16, COL_MUTED);
        DrawRectangleRounded((Rectangle){ x - 11, y - 3, 22, 17 }, 0.25f, 6, COL_MUTED);
    }

    if (isSelected) DrawRing(c, NODE_R + 4, NODE_R + 8, 0, 360, 48, RAYWHITE);

    const Level *level = LevelGet(i);
    UiDrawCenteredText(level->title, (int)x, (int)y + NODE_R + 14, 18, i <= done ? RAYWHITE : COL_MUTED);
}

static void DrawMap(int done, int n, float t, Vector2 mouse, bool inMap)
{
    BeginScissorMode(0, HUD_H, MAP_W, SCREEN_H - HUD_H);

    // Path between nodes
    for (int i = 0; i < n - 1; i++)
    {
        Vector2 a = { nodeX[i], ScreenY(i) };
        Vector2 b = { nodeX[i + 1], ScreenY(i + 1) };
        DrawLineEx(a, b, 8.0f, (i + 1 <= done) ? COL_DONE : COL_LOCKED);
    }

    // Section banners
    int prevSection = -1;
    for (int i = 0; i < n; i++)
    {
        const Level *level = LevelGet(i);
        if (level->section == prevSection) continue;
        prevSection = level->section;

        Rectangle banner = { MAP_W / 2.0f - 170, ScreenY(i) - 100, 340, 38 };
        DrawRectangleRounded(banner, 0.5f, 8, Fade(SectionColor(level->section), 0.9f));
        UiDrawCenteredText(SectionName(level->section), MAP_W / 2, (int)banner.y + 9, 20, COL_BG);
    }

    for (int i = 0; i < n; i++) DrawNode(i, done, i == selected, t);

    // The cart waits next to the current level and bobs up and down
    if (done < n)
    {
        float bob = sinf(t * 5.0f) * 3.0f;
        UiDrawCartAt(nodeX[done] + 74.0f, ScreenY(done) + 4.0f + bob, 0.0f, 1.0f);
    }

    EndScissorMode();

    // Click a node to select it
    if (inMap && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        for (int i = 0; i < n; i++)
        {
            if (CheckCollisionPointCircle(mouse, (Vector2){ nodeX[i], ScreenY(i) }, NODE_R + 6)) selected = i;
        }
    }
}

static void DrawHud(AppState *app, Account *acc, int done, int n)
{
    DrawRectangle(0, 0, SCREEN_W, HUD_H, COL_PANEL);

    if (UiButton((Rectangle){ 20, 16, 120, 48 }, "Menu", true, 1.0f) || IsKeyPressed(KEY_ESCAPE))
    {
        AppGoTo(app, SCREEN_MENU);
    }

    UiDrawText(acc->fullName, 170, 18, 24, RAYWHITE);
    char sub[96];
    snprintf(sub, sizeof(sub), "@%s  |  %s", acc->username, ExperienceName(acc->experience));
    UiDrawText(sub, 170, 48, 16, COL_MUTED);

    // Progress bar and XP
    char prog[32];
    snprintf(prog, sizeof(prog), "%d / %d levels", done, n);
    UiDrawText(prog, 800, 16, 18, COL_MUTED);
    Rectangle bar = { 800, 44, 250, 14 };
    DrawRectangleRounded(bar, 0.5f, 8, COL_BG);
    if (done > 0) DrawRectangleRounded((Rectangle){ bar.x, bar.y, bar.width * done / n, bar.height }, 0.5f, 8, COL_DONE);

    char xp[32];
    snprintf(xp, sizeof(xp), "%d XP", acc->xp);
    UiDrawText(xp, 1100, 26, 28, COL_CYAN);
}

static void DrawDetailsPanel(AppState *app, Account *acc, int done, int n)
{
    Rectangle panel = { 900, 100, 360, 600 };
    DrawRectangleRounded(panel, 0.05f, 8, COL_PANEL);

    const Level *level = LevelGet(selected);
    float x = panel.x + 24;

    UiDrawText(SectionName(level->section), (int)x, 124, 18, SectionColor(level->section));
    UiDrawText(level->title, (int)x, 152, 28, RAYWHITE);

    char line[64];
    snprintf(line, sizeof(line), "Level %d of %d", selected + 1, n);
    UiDrawText(line, (int)x, 192, 18, COL_MUTED);

    UiDrawWrappedText(level->summary, (Rectangle){ x, 232, 312, 200 }, 20, RAYWHITE);

    snprintf(line, sizeof(line), "Reward: +%d XP", level->xp);
    UiDrawText(line, (int)x, 440, 20, COL_CYAN);

    const char *status;
    Color statusColor;
    if (selected < done)        { status = "Completed";  statusColor = COL_DONE; }
    else if (selected == done)  { status = "Ready to start"; statusColor = COL_ACCENT; }
    else                        { status = "Locked: finish the earlier levels first"; statusColor = COL_MUTED; }
    UiDrawText(status, (int)x, 472, 18, statusColor);

    bool unlocked = selected <= done;
    const char *startLabel = selected < done ? "Review Lesson" : (selected == done ? "Start Lesson" : "Locked");
    if (UiButton((Rectangle){ x, 520, 312, 52 }, startLabel, unlocked, 1.0f))
    {
        if (QuestionCount(selected) == 0)
        {
            UiToast("No questions for this level yet");
        }
        else
        {
            app->activeLevel = selected;
            AppGoTo(app, SCREEN_LESSON);
            return;
        }
    }

#if DEV_TOOLS
    // Test buttons so progress and CSV saving can be tried before lessons exist
    if (UiButton((Rectangle){ x, 584, 312, 40 }, "Mark complete (dev)", selected == done && done < n, 1.0f))
    {
        acc->levelsCompleted++;
        acc->xp += level->xp;
        AppSaveAccounts(app);

        char msg[64];
        snprintf(msg, sizeof(msg), "+%d XP earned!", level->xp);
        UiToast(msg);

        if (selected + 1 < n) selected++;
        CenterOn(selected);
    }
    if (UiButton((Rectangle){ x, 636, 312, 40 }, "Reset progress (dev)", true, 1.0f))
    {
        acc->levelsCompleted = 0;
        acc->xp = 0;
        selected = 0;
        AppSaveAccounts(app);
        CenterOn(0);
        UiToast("Progress reset");
    }
#else
    (void)app;
    (void)acc;
#endif
}

void ScreenMapRun(AppState *app, float dt)
{
    Account *acc = AppCurrentAccount(app);
    if (acc == NULL)
    {
        AppGoTo(app, SCREEN_MENU);
        return;
    }

    int n = LevelCount();
    if (n > MAX_NODES) n = MAX_NODES;
    int done = acc->levelsCompleted;
    if (done > n) done = n;

    Vector2 mouse = GetMousePosition();
    bool inMap = mouse.x < MAP_W && mouse.y > HUD_H;

    // Scrolling: mouse wheel over the map, or the arrow keys
    if (inMap) targetScroll -= GetMouseWheelMove() * 70.0f;
    if (IsKeyDown(KEY_DOWN)) targetScroll += 500.0f * dt;
    if (IsKeyDown(KEY_UP))   targetScroll -= 500.0f * dt;
    targetScroll = ClampScroll(targetScroll);
    scroll += (targetScroll - scroll) * fminf(1.0f, dt * 10.0f);

    DrawMap(done, n, (float)GetTime(), mouse, inMap);
    DrawDetailsPanel(app, acc, done, n);
    DrawHud(app, acc, done, n);
}