// Code practice: the list of problems.
#include "screens.h"
#include "theme.h"
#include "ui.h"
#include "problems.h"
#include "judge.h"
#include "practice_progress.h"

#include <stdio.h>

#define VISIBLE_ROWS 7
#define ROW_H 64.0f
#define ROW_GAP 8.0f

static int firstShown;
static bool compilerOk;

void ScreenPracticeEnter(AppState *app)
{
    (void)app;
    firstShown = 0;
    PracticeLoad(PRACTICE_FILE);
    compilerOk = JudgeCompilerAvailable();
}

void ScreenPracticeRun(AppState *app, float dt)
{
    (void)dt;
    Account *acc = AppCurrentAccount(app);
    const char *user = acc ? acc->username : "guest";
    int total = ProblemCount();
    int solved = PracticeSolvedCount(user);

    UiDrawText("Code Practice", 60, 28, 40, RAYWHITE);
    UiDrawText("Write real C++ and check it against test cases.", 62, 82, 18, COL_MUTED);

    char line[64];
    snprintf(line, sizeof(line), "%d / %d solved", solved, total);
    UiDrawText(line, 840, 30, 20, COL_MUTED);
    Rectangle bar = { 840, 62, 200, 12 };
    DrawRectangleRounded(bar, 0.5f, 8, COL_PANEL);
    if (solved > 0) DrawRectangleRounded((Rectangle){ bar.x, bar.y, bar.width * solved / total, bar.height }, 0.5f, 8, COL_DONE);

    if (UiButton((Rectangle){ SCREEN_W - 180, 28, 120, 46 }, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE))
    {
        AppGoTo(app, SCREEN_MENU);
        return;
    }

    if (!compilerOk)
    {
        Rectangle banner = { 60, 112, 1160, 40 };
        DrawRectangleRounded(banner, 0.3f, 8, Fade(COL_ERROR, 0.2f));
        UiDrawText("No C++ compiler found. Install GCC or MinGW-w64 and add g++ to your PATH to run tests.", 80, 120, 16, COL_ERROR);
    }

    int maxFirst = total > VISIBLE_ROWS ? total - VISIBLE_ROWS : 0;
    firstShown -= (int)GetMouseWheelMove();
    if (firstShown < 0) firstShown = 0;
    if (firstShown > maxFirst) firstShown = maxFirst;

    Vector2 mouse = GetMousePosition();
    for (int k = 0; k < VISIBLE_ROWS; k++)
    {
        int idx = firstShown + k;
        const Problem *p = ProblemGet(idx);
        if (p == NULL) break;

        Rectangle row = { 60, 166.0f + k * (ROW_H + ROW_GAP), 1160, ROW_H };
        bool hovered = CheckCollisionPointRec(mouse, row);
        bool done = PracticeIsSolved(user, p->id);

        DrawRectangleRounded(row, 0.2f, 8, hovered ? (Color){ 38, 46, 86, 255 } : COL_PANEL);
        DrawRectangleRoundedLinesEx(row, 0.2f, 8, 2.0f, hovered ? COL_ACCENT : Fade(COL_MUTED, 0.25f));

        // Number or tick
        Vector2 badge = { row.x + 36, row.y + ROW_H / 2 };
        DrawCircleV(badge, 18, done ? COL_DONE : COL_LOCKED);
        if (done)
        {
            DrawLineEx((Vector2){ badge.x - 8, badge.y }, (Vector2){ badge.x - 2, badge.y + 7 }, 3.5f, RAYWHITE);
            DrawLineEx((Vector2){ badge.x - 2, badge.y + 7 }, (Vector2){ badge.x + 9, badge.y - 7 }, 3.5f, RAYWHITE);
        }
        else
        {
            char num[8];
            snprintf(num, sizeof(num), "%d", idx + 1);
            UiDrawText(num, (int)badge.x - UiMeasureText(num, 16) / 2, (int)badge.y - 12, 16, RAYWHITE);
        }

        UiDrawText(p->title, (int)row.x + 76, (int)row.y + 8, 22, RAYWHITE);
        snprintf(line, sizeof(line), "%s  |  %s", p->topic, p->source);
        UiDrawText(line, (int)row.x + 76, (int)row.y + 38, 15, COL_MUTED);

        // Difficulty pill
        Color pill = p->difficulty == DIFF_EASY ? COL_DONE : COL_ACCENT;
        const char *diff = DifficultyName(p->difficulty);
        int pw = UiMeasureText(diff, 16) + 28;
        Rectangle pillRect = { row.x + 800, row.y + 18, (float)pw, 28 };
        DrawRectangleRounded(pillRect, 0.5f, 8, Fade(pill, 0.2f));
        UiDrawText(diff, (int)pillRect.x + 14, (int)pillRect.y + 3, 16, pill);

        snprintf(line, sizeof(line), "+%d XP", p->xp);
        UiDrawText(line, (int)row.x + 940, (int)row.y + 18, 18, COL_CYAN);

        UiDrawText(done ? "Solved" : "Not solved", (int)(row.x + row.width - 24 - UiMeasureText(done ? "Solved" : "Not solved", 16)),
                   (int)row.y + 20, 16, done ? COL_DONE : COL_MUTED);

        if (hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            app->activeProblem = idx;
            AppGoTo(app, SCREEN_CODE);
            return;
        }
    }

    if (total > VISIBLE_ROWS) UiDrawCenteredText("Scroll to see more problems", SCREEN_W / 2, 690, 16, COL_MUTED);
}