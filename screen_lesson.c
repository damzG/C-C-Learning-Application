// Lesson screen: asks the questions for one level, gives feedback, then shows a result.
#include "screens.h"
#include "theme.h"
#include "ui.h"
#include "levels.h"
#include "questions.h"

#include <stdio.h>

typedef enum { PHASE_ASK, PHASE_RESULT } Phase;

static Phase phase;
static int level;
static int total;
static int qIndex;
static int score;
static int chosen;
static bool answered;
static bool hintShown;
static bool right[LESSON_MAX_QUESTIONS];
static bool passed;
static int xpEarned;

static void ResetLesson(void)
{
    phase = PHASE_ASK;
    total = QuestionCount(level);
    qIndex = 0;
    score = 0;
    chosen = -1;
    answered = false;
    hintShown = false;
    passed = false;
    xpEarned = 0;
    for (int i = 0; i < LESSON_MAX_QUESTIONS; i++) right[i] = false;
}

void ScreenLessonEnter(AppState *app)
{
    level = app->activeLevel;
    ResetLesson();
}

static void FinishLesson(AppState *app)
{
    phase = PHASE_RESULT;
    passed = LessonPassed(score, total);

    Account *acc = AppCurrentAccount(app);
    if (passed && acc && AccountCompleteLevel(acc, level, LevelGet(level)->xp))
    {
        xpEarned = LevelGet(level)->xp;
        AppSaveAccounts(app);
    }
}

static void Answer(const Question *q, int option)
{
    answered = true;
    chosen = option;
    right[qIndex] = (option == q->correct);
    if (right[qIndex]) score++;
}

static void NextQuestion(AppState *app)
{
    qIndex++;
    answered = false;
    chosen = -1;
    hintShown = false;
    if (qIndex >= total) FinishLesson(app);
}

static void DrawHud(AppState *app)
{
    DrawRectangle(0, 0, SCREEN_W, 70, COL_PANEL);

    if (UiButton((Rectangle){ 20, 12, 110, 46 }, "Quit", true, 1.0f) || IsKeyPressed(KEY_ESCAPE))
    {
        AppGoTo(app, SCREEN_MAP);
        return;
    }

    char title[96];
    snprintf(title, sizeof(title), "Level %d: %s", level + 1, LevelGet(level)->title);
    DrawText(title, 160, 22, 26, RAYWHITE);

    if (phase == PHASE_ASK)
    {
        char prog[32];
        snprintf(prog, sizeof(prog), "Question %d of %d", qIndex + 1, total);
        DrawText(prog, 940, 14, 18, COL_MUTED);

        Rectangle bar = { 940, 42, 300, 12 };
        DrawRectangleRounded(bar, 0.5f, 8, COL_BG);
        float filled = (float)qIndex + (answered ? 1.0f : 0.0f);
        if (filled > 0) DrawRectangleRounded((Rectangle){ bar.x, bar.y, bar.width * filled / total, bar.height }, 0.5f, 8, COL_DONE);
    }
}

static void RunQuestion(AppState *app)
{
    const Question *q = QuestionGet(level, qIndex);
    if (q == NULL)
    {
        AppGoTo(app, SCREEN_MAP);
        return;
    }

    Color tagColor = (q->kind == Q_BUG) ? COL_ERROR : (q->kind == Q_OUTPUT ? COL_CYAN : COL_ACCENT);
    const char *tag = QuestionKindName(q->kind);
    int tagW = MeasureText(tag, 16) + 28;
    DrawRectangleRounded((Rectangle){ 80, 92, (float)tagW, 28 }, 0.5f, 8, Fade(tagColor, 0.25f));
    DrawText(tag, 94, 98, 16, tagColor);

    float y = 132;
    y += UiDrawWrappedText(q->prompt, (Rectangle){ 80, y, 1120, 80 }, 24, RAYWHITE) + 6;

    if (q->code != NULL)
    {
        float codeH = UiCodeLineCount(q->code) * 26.0f + 24.0f;
        UiDrawCodeBlock(q->code, (Rectangle){ 80, y, 1120, codeH });
        y += codeH + 18;
    }

    // Answer options
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < OPTION_COUNT; i++)
    {
        Rectangle r = { 80, y + i * 60.0f, 1120, 52 };
        bool hovered = !answered && CheckCollisionPointRec(mouse, r);

        Color fill = COL_PANEL;
        Color border = Fade(COL_MUTED, 0.5f);
        bool dim = false;

        if (answered)
        {
            if (i == q->correct)      { fill = (Color){ 30, 80, 55, 255 }; border = COL_DONE; }
            else if (i == chosen)     { fill = (Color){ 90, 35, 45, 255 }; border = COL_ERROR; }
            else                      { border = Fade(COL_MUTED, 0.2f); dim = true; }
        }
        else if (hovered)
        {
            border = COL_ACCENT;
        }

        DrawRectangleRounded(r, 0.25f, 8, fill);
        DrawRectangleRoundedLinesEx(r, 0.25f, 8, 2.0f, border);

        Vector2 badge = { r.x + 32, r.y + 26 };
        DrawCircleV(badge, 16, Fade(border, 0.9f));
        char letter[2] = { (char)('A' + i), '\0' };
        DrawText(letter, (int)badge.x - MeasureText(letter, 20) / 2, (int)badge.y - 10, 20, COL_BG);

        DrawText(q->options[i], (int)r.x + 66, (int)r.y + 15, 22, dim ? COL_MUTED : RAYWHITE);

        if (!answered && ((hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_ONE + i)))
        {
            Answer(q, i);
        }
    }

    // Bottom panel: hint before answering, feedback after
    Rectangle panel = { 80, 646, 1120, 64 };
    DrawRectangleRounded(panel, 0.2f, 8, COL_PANEL);

    if (!answered)
    {
        if (hintShown)
        {
            char hint[200];
            snprintf(hint, sizeof(hint), "Hint: %s", q->hint);
            UiDrawWrappedText(hint, (Rectangle){ 100, 668, 880, 40 }, 20, COL_CYAN);
        }
        else
        {
            DrawText("Choose an answer, or press 1 to 4.", 100, 668, 20, COL_MUTED);
        }

        if (UiButton((Rectangle){ 1020, 653, 170, 50 }, "Show hint", !hintShown, 1.0f)) hintShown = true;
    }
    else
    {
        bool ok = right[qIndex];
        DrawText(ok ? "Correct!" : "Not quite.", 100, 654, 22, ok ? COL_DONE : COL_ERROR);
        UiDrawWrappedText(q->explanation, (Rectangle){ 100, 682, 900, 24 }, 16, RAYWHITE);

        const char *nextLabel = (qIndex + 1 < total) ? "Next" : "See result";
        if (UiButton((Rectangle){ 1020, 653, 170, 50 }, nextLabel, true, 1.0f) || IsKeyPressed(KEY_ENTER))
        {
            NextQuestion(app);
        }
    }
}

static void RunResult(AppState *app)
{
    char line[128];

    UiDrawCenteredText(passed ? "Lesson complete!" : "Almost there!", SCREEN_W / 2, 130, 52, passed ? COL_DONE : COL_ACCENT);

    snprintf(line, sizeof(line), "%d / %d correct", score, total);
    UiDrawCenteredText(line, SCREEN_W / 2, 210, 36, RAYWHITE);

    // One dot per question
    float startX = SCREEN_W / 2.0f - (total - 1) * 22.0f;
    for (int i = 0; i < total; i++)
    {
        DrawCircleV((Vector2){ startX + i * 44.0f, 290 }, 14, right[i] ? COL_DONE : COL_ERROR);
    }

    if (passed && xpEarned > 0)
    {
        snprintf(line, sizeof(line), "+%d XP earned. The next level is unlocked!", xpEarned);
        UiDrawCenteredText(line, SCREEN_W / 2, 340, 24, COL_CYAN);
    }
    else if (passed)
    {
        UiDrawCenteredText("Great practice. XP is only awarded the first time.", SCREEN_W / 2, 340, 22, COL_MUTED);
    }
    else
    {
        snprintf(line, sizeof(line), "You need at least %d correct to pass. Have another go!", LessonPassMark(total));
        UiDrawCenteredText(line, SCREEN_W / 2, 340, 22, COL_MUTED);
    }

    Account *acc = AppCurrentAccount(app);
    if (acc && acc->levelsCompleted >= LevelCount())
    {
        UiDrawCenteredText("You have completed every level on the map!", SCREEN_W / 2, 380, 22, COL_ACCENT);
    }

    if (UiButton((Rectangle){ SCREEN_W / 2.0f - 270, 450, 250, 56 }, "Back to Map", true, 1.0f))
    {
        AppGoTo(app, SCREEN_MAP);
    }
    if (UiButton((Rectangle){ SCREEN_W / 2.0f + 20, 450, 250, 56 }, passed ? "Practice Again" : "Try Again", true, 1.0f))
    {
        ResetLesson();
    }
}

void ScreenLessonRun(AppState *app, float dt)
{
    (void)dt;

    if (total == 0 || LevelGet(level) == NULL)
    {
        AppGoTo(app, SCREEN_MAP);
        return;
    }

    if (phase == PHASE_ASK) RunQuestion(app);
    else                    RunResult(app);

    DrawHud(app);
}