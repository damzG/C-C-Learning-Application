#include "screens.h"
#include "theme.h"
#include "ui.h"
#include "avatar_draw.h"

#include <stdio.h>
#include <string.h>

#define FIELD_COUNT 4
#define FIELD_NAME     0
#define FIELD_USERNAME 1
#define FIELD_EMAIL    2
#define FIELD_COURSE   3

static UiTextBox boxes[FIELD_COUNT];
static int focus;
static int experience;
static char errorMessage[96];

void ScreenCreateEnter(AppState *app)
{
    (void)app;
    const float x = 440.0f, w = 400.0f, h = 44.0f;

    UiTextBoxInit(&boxes[FIELD_NAME],     (Rectangle){ x, 170, w, h }, "Full name", "e.g. Ada Lovelace");
    UiTextBoxInit(&boxes[FIELD_USERNAME], (Rectangle){ x, 250, w, h }, "Username", "3 to 20 letters, numbers or _");
    UiTextBoxInit(&boxes[FIELD_EMAIL],    (Rectangle){ x, 330, w, h }, "Email", "name@example.com");
    UiTextBoxInit(&boxes[FIELD_COURSE],   (Rectangle){ x, 410, w, h }, "Course or school", "e.g. Advanced Programming");

    focus = 0;
    experience = EXP_BEGINNER;
    errorMessage[0] = '\0';
}

static void Submit(AppState *app)
{
    Account a;
    memset(&a, 0, sizeof(a));
    snprintf(a.fullName, FIELD_LEN, "%s", boxes[FIELD_NAME].text);
    snprintf(a.username, FIELD_LEN, "%s", boxes[FIELD_USERNAME].text);
    snprintf(a.email,    FIELD_LEN, "%s", boxes[FIELD_EMAIL].text);
    snprintf(a.course,   FIELD_LEN, "%s", boxes[FIELD_COURSE].text);
    a.experience = experience;
    a.xp = 0;
    a.levelsCompleted = 0;
    AccountTodayString(a.created);
    AccountTodayString(a.lastLogin);

    const char *problem = AccountValidate(&app->accounts, &a);
    if (problem)
    {
        snprintf(errorMessage, sizeof(errorMessage), "%s", problem);
        return;
    }

    int index = AccountsAdd(&app->accounts, &a);
    if (index < 0)
    {
        snprintf(errorMessage, sizeof(errorMessage), "Profile limit reached.");
        return;
    }

    app->current = index;
    AppSaveAccounts(app);

    char msg[96];
    snprintf(msg, sizeof(msg), "Welcome, %s!", a.fullName);
    UiToast(msg);
    AppGoTo(app, SCREEN_MAP);
}

void ScreenCreateRun(AppState *app, float dt)
{
    (void)dt;

    UiDrawCenteredText("Create your profile", SCREEN_W / 2, 50, 40, RAYWHITE);
    UiDrawCenteredText("Saved on this computer in accounts.csv", SCREEN_W / 2, 102, 18, COL_MUTED);

    // Focus: click a box, or press Tab / Shift+Tab
    for (int i = 0; i < FIELD_COUNT; i++)
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), boxes[i].rect)) focus = i;
    }
    if (IsKeyPressed(KEY_TAB))
    {
        bool back = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        focus = (focus + (back ? FIELD_COUNT - 1 : 1)) % FIELD_COUNT;
    }

    UiTextBoxUpdate(&boxes[focus]);
    for (int i = 0; i < FIELD_COUNT; i++) UiTextBoxDraw(&boxes[i], i == focus);

    // Live preview of the generated profile picture. It changes as the username is typed.
    const char *previewName = boxes[FIELD_USERNAME].len > 0 ? boxes[FIELD_USERNAME].text : "?";
    AvatarDraw(previewName, 920, 170, 160, 1.0f);
    UiDrawCenteredText("Your profile picture", 1000, 345, 20, RAYWHITE);
    UiDrawCenteredText("Made from your username", 1000, 372, 16, COL_MUTED);

    // Experience selector
    DrawText("Experience level", 440, 478, 18, COL_MUTED);
    for (int e = 0; e < EXP_COUNT; e++)
    {
        Rectangle r = { 440.0f + e * 137.0f, 502, 126, 44 };
        bool hovered = CheckCollisionPointRec(GetMousePosition(), r);
        bool selected = (experience == e);

        DrawRectangleRounded(r, 0.3f, 8, selected ? COL_ACCENT : COL_PANEL);
        DrawRectangleRoundedLinesEx(r, 0.3f, 8, 2.0f, (selected || hovered) ? COL_ACCENT : Fade(COL_MUTED, 0.5f));

        const char *name = ExperienceName(e);
        int tw = MeasureText(name, 18);
        DrawText(name, (int)(r.x + (r.width - tw) / 2), (int)r.y + 13, 18, selected ? COL_BG : RAYWHITE);

        if (hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) experience = e;
    }

    if (errorMessage[0]) UiDrawCenteredText(errorMessage, SCREEN_W / 2, 560, 20, COL_ERROR);

    if (UiButton((Rectangle){ 440, 592, 400, 54 }, "Create Profile", true, 1.0f) || IsKeyPressed(KEY_ENTER))
    {
        Submit(app);
    }

    if (UiButton((Rectangle){ 30, 30, 120, 44 }, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE))
    {
        AppGoTo(app, app->accounts.count > 0 ? SCREEN_PROFILES : SCREEN_MENU);
    }
}