// Code practice: description, hints, editor and test results for one problem.
#include "screens.h"
#include "theme.h"
#include "ui.h"
#include "editor.h"
#include "problems.h"
#include "judge.h"
#include "practice_progress.h"


#include <stdio.h>
#include <string.h>

#define MAX_PROBLEMS 64
#define SOLUTION_UNLOCK_RUNS 2

// The editor text for each problem is kept while the app is open
static char drafts[MAX_PROBLEMS][EDITOR_MAX];
static bool draftSaved[MAX_PROBLEMS];

static Editor editor;
static int problemIndex;
static const Problem *problem;
static int hintsShown;
static int failedRuns;
static bool runPending;
static bool hasResult;
static JudgeResult result;
static int selectedTest;
static bool showSolution;

static void SaveDraft(void)
{
    if (problemIndex < 0 || problemIndex >= MAX_PROBLEMS) return;
    memcpy(drafts[problemIndex], editor.text, EDITOR_MAX);
    draftSaved[problemIndex] = true;
}

void ScreenCodeEnter(AppState *app)
{
    problemIndex = app->activeProblem;
    problem = ProblemGet(problemIndex);

    EditorInit(&editor, (Rectangle){ 490, 84, 770, 386 });
    if (problem != NULL)
    {
        bool restore = problemIndex < MAX_PROBLEMS && draftSaved[problemIndex];
        EditorSetText(&editor, restore ? drafts[problemIndex] : problem->starter);
    }

    hintsShown = 0;
    failedRuns = 0;
    runPending = false;
    hasResult = false;
    selectedTest = 0;
    showSolution = false;
    PracticeLoad(PRACTICE_FILE);
}

static void RunTests(AppState *app, Account *acc, const char *user)
{
    SaveDraft();
    JudgeRun(problem, editor.text, &result);
    hasResult = true;

    selectedTest = 0;
    for (int t = 0; t < result.testCount; t++)
    {
        if (result.verdict[t] != TEST_PASS)
        {
            selectedTest = t;
            break;
        }
    }

    bool allPassed = result.status == JUDGE_OK && result.passed == result.testCount;
    if (allPassed)
    {
        if (PracticeMarkSolved(user, problem->id, PRACTICE_FILE))
        {
            char msg[64];
            if (acc)
            {
                acc->xp += problem->xp;
                AppSaveAccounts(app);
                snprintf(msg, sizeof(msg), "Solved! +%d XP", problem->xp);
            }
            else
            {
                snprintf(msg, sizeof(msg), "Solved! Sign in to earn XP");
            }
            UiToast(msg);
        }
        else
        {
            UiToast("All tests passed!");
        }
    }
    else if (result.status == JUDGE_OK || result.status == JUDGE_COMPILE_ERROR)
    {
        failedRuns++;
    }
}

// ---------------------------------------------------------------------------
// Drawing pieces
// ---------------------------------------------------------------------------
static void DrawLeftPanel(bool ui, bool solved)
{
    Rectangle panel = { 20, 84, 450, 616 };
    DrawRectangleRounded(panel, 0.03f, 8, COL_PANEL);

    float x = panel.x + 22;
    char line[96];
    snprintf(line, sizeof(line), "%s  |  %s", problem->source, problem->topic);
    UiDrawText(line, (int)x, 100, 15, COL_MUTED);

    float used = UiDrawWrappedText(problem->description, (Rectangle){ x, 130, 406, 250 }, 16, RAYWHITE);
    (void)used;

    // Hints
    DrawLine((int)x, 396, (int)(x + 406), 396, Fade(COL_MUTED, 0.3f));
    UiDrawText("Hints", (int)x, 404, 18, COL_CYAN);

    float y = 434;
    if (hintsShown == 0)
    {
        UiDrawWrappedText("Stuck? Reveal a hint. The solution unlocks after 2 failed runs or 3 hints.",
                          (Rectangle){ x, y, 406, 80 }, 15, COL_MUTED);
    }
    for (int i = 0; i < hintsShown; i++)
    {
        snprintf(line, sizeof(line), "%d. %s", i + 1, problem->hints[i]);
        y += UiDrawWrappedText(line, (Rectangle){ x, y, 406, 80 }, 15, RAYWHITE) + 4;
    }

    const char *hintLabel = hintsShown < HINT_COUNT ? TextFormat("Show hint (%d/%d)", hintsShown + 1, HINT_COUNT) : "No more hints";
    if (UiButton((Rectangle){ x, 634, 196, 50 }, hintLabel, ui && hintsShown < HINT_COUNT, 1.0f)) hintsShown++;

    bool unlocked = failedRuns >= SOLUTION_UNLOCK_RUNS || hintsShown >= HINT_COUNT || solved;
    if (UiButton((Rectangle){ x + 210, 634, 196, 50 }, unlocked ? "See solution" : "Solution locked", ui && unlocked, 1.0f))
    {
        showSolution = true;
    }
}

static void CompactInput(const char *in, char *out, size_t cap)
{
    size_t n = 0;
    for (const char *p = in; *p && n + 4 < cap; p++)
    {
        if (*p == '\n')
        {
            if (p[1] == '\0') break;
            out[n++] = ' ';
            out[n++] = '|';
            out[n++] = ' ';
        }
        else
        {
            out[n++] = *p;
        }
    }
    out[n] = '\0';
}

static void DrawResultPanel(bool ui)
{
    Rectangle panel = { 490, 482, 770, 218 };
    DrawRectangleRounded(panel, 0.04f, 8, COL_PANEL);
    float x = panel.x + 22;

    if (!hasResult)
    {
        char msg[96];
        snprintf(msg, sizeof(msg), "Press Run Tests (Ctrl+Enter) to check your code against %d tests.", problem->testCount);
        UiDrawText(msg, (int)x, 504, 18, COL_MUTED);
        UiDrawText("Your code is compiled with g++ and run on each test.", (int)x, 536, 15, COL_MUTED);
        return;
    }

    if (result.status == JUDGE_NO_COMPILER)
    {
        UiDrawText("g++ was not found", (int)x, 500, 22, COL_ERROR);
        UiDrawWrappedText("Install GCC (or MinGW-w64 on Windows), add it to your PATH, then restart the app.",
                          (Rectangle){ x, 540, 726, 100 }, 17, RAYWHITE);
        return;
    }
    if (result.status == JUDGE_COMPILE_ERROR || result.status == JUDGE_INTERNAL)
    {
        UiDrawText(result.status == JUDGE_COMPILE_ERROR ? "Compile error" : "Could not run", (int)x, 500, 22, COL_ERROR);
        UiDrawWrappedText(result.compilerOutput, (Rectangle){ x, 540, 726, 150 }, 17, RAYWHITE);
        return;
    }

    bool all = result.passed == result.testCount;
    char head[64];
    if (all) snprintf(head, sizeof(head), "All %d tests passed!", result.testCount);
    else     snprintf(head, sizeof(head), "%d of %d tests passed", result.passed, result.testCount);
    UiDrawText(head, (int)x, 498, 24, all ? COL_DONE : COL_ACCENT);

    // One chip per test
    for (int t = 0; t < result.testCount; t++)
    {
        Vector2 c = { x + 20 + t * 46.0f, 556 };
        TestVerdict v = result.verdict[t];
        Color col = v == TEST_PASS ? COL_DONE : (v == TEST_TIMEOUT ? COL_ACCENT : (v == TEST_SKIPPED ? COL_LOCKED : COL_ERROR));

        DrawCircleV(c, 17, col);
        if (t == selectedTest) DrawRing(c, 20, 23, 0, 360, 36, RAYWHITE);

        char num[8];
        snprintf(num, sizeof(num), "%d", t + 1);
        UiDrawText(num, (int)c.x - UiMeasureText(num, 15) / 2, (int)c.y - 11, 15, v == TEST_SKIPPED ? COL_MUTED : COL_BG);

        if (ui && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointCircle(GetMousePosition(), c, 20)) selectedTest = t;
    }

    // Details for the selected test
    int t = selectedTest;
    TestVerdict v = result.verdict[t];
    char buf[160];
    float y = 592;

    if (v == TEST_PASS)
    {
        UiDrawText("This test passed.", (int)x, (int)y, 17, COL_DONE);
    }
    else if (v == TEST_TIMEOUT)
    {
        UiDrawWrappedText("Your code ran for more than 2 seconds. Look for an infinite loop, or a faster approach.",
                          (Rectangle){ x, y, 726, 80 }, 17, COL_ACCENT);
    }
    else if (v == TEST_SKIPPED)
    {
        UiDrawText("Not run because an earlier test timed out.", (int)x, (int)y, 17, COL_MUTED);
    }
    else
    {
        char input[96];
        CompactInput(problem->tests[t].input, input, sizeof(input));
        snprintf(buf, sizeof(buf), "Input:      %s", input[0] ? input : "(empty)");
        UiDrawMono(buf, (int)x, (int)y, 15, COL_MUTED);
        snprintf(buf, sizeof(buf), "Expected:   %s", problem->tests[t].expected[0] ? problem->tests[t].expected : "(nothing)");
        UiDrawMono(buf, (int)x, (int)y + 28, 15, COL_DONE);
        if (v == TEST_CRASH)
        {
            UiDrawMono("Your program crashed. Check for invalid memory access.", (int)x, (int)y + 56, 15, COL_ERROR);
        }
        else
        {
            snprintf(buf, sizeof(buf), "Your output: %s", result.actual[t][0] ? result.actual[t] : "(nothing printed)");
            UiDrawMono(buf, (int)x, (int)y + 56, 15, COL_ERROR);
        }
    }
}

static void DrawSolutionOverlay(void)
{
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){ 0, 0, 0, 170 });

    Rectangle panel = { 140, 80, 1000, 560 };
    DrawRectangleRounded(panel, 0.03f, 8, COL_PANEL);
    UiDrawText("Reference solution", 170, 98, 28, RAYWHITE);
    UiDrawText("Read it, then try to write it again yourself without looking.", 172, 138, 16, COL_MUTED);

    float h = UiCodeLineCount(problem->solution) * 26.0f + 24.0f;
    UiDrawCodeBlock(problem->solution, (Rectangle){ 170, 170, 940, h });

    if (UiButtonPrimary((Rectangle){ 170, 568, 230, 52 }, "Copy to editor", true, 1.0f))
    {
        EditorSetText(&editor, problem->solution);
        showSolution = false;
    }
    if (UiButton((Rectangle){ 420, 568, 150, 52 }, "Close", true, 1.0f)) showSolution = false;
}

// ---------------------------------------------------------------------------
void ScreenCodeRun(AppState *app, float dt)
{
    (void)dt;
    if (problem == NULL)
    {
        AppGoTo(app, SCREEN_PRACTICE);
        return;
    }

    Account *acc = AppCurrentAccount(app);
    const char *user = acc ? acc->username : "guest";

    // The run was requested last frame, so the "Running" message is already on screen
    if (runPending)
    {
        RunTests(app, acc, user);
        runPending = false;
    }

    bool ui = !showSolution && !runPending;
    bool solved = PracticeIsSolved(user, problem->id);

    // Top bar
    DrawRectangle(0, 0, SCREEN_W, 70, COL_PANEL);
    if (UiButton((Rectangle){ 20, 12, 140, 46 }, "Problems", !runPending, 1.0f) || (ui && IsKeyPressed(KEY_ESCAPE)))
    {
        SaveDraft();
        AppGoTo(app, SCREEN_PRACTICE);
        return;
    }

    char title[96];
    snprintf(title, sizeof(title), "%d. %s", problemIndex + 1, problem->title);
    UiDrawText(title, 182, 18, 28, RAYWHITE);

    Color pill = problem->difficulty == DIFF_EASY ? COL_DONE : COL_ACCENT;
    const char *diff = DifficultyName(problem->difficulty);
    Rectangle pillRect = { 182.0f + UiMeasureText(title, 28) + 18, 22, (float)UiMeasureText(diff, 15) + 24, 26 };
    DrawRectangleRounded(pillRect, 0.5f, 8, Fade(pill, 0.2f));
    UiDrawText(diff, (int)pillRect.x + 12, (int)pillRect.y + 3, 15, pill);
    if (solved) UiDrawText("Solved", (int)(pillRect.x + pillRect.width + 14), 26, 16, COL_DONE);

    if (UiButton((Rectangle){ 960, 12, 120, 46 }, "Reset", ui, 1.0f)) EditorSetText(&editor, problem->starter);

    bool ctrlEnter = ui && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_ENTER);
    if (UiButtonPrimary((Rectangle){ 1094, 12, 166, 46 }, "Run Tests", ui, 1.0f) || ctrlEnter) runPending = true;

    DrawLeftPanel(ui, solved);

    EditorUpdate(&editor, ui);
    EditorDraw(&editor, ui);

    DrawResultPanel(ui);

    if (runPending)
    {
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){ 0, 0, 0, 150 });
        Rectangle box = { SCREEN_W / 2.0f - 230, SCREEN_H / 2.0f - 50, 460, 100 };
        DrawRectangleRounded(box, 0.2f, 8, COL_PANEL);
        DrawRectangleRoundedLinesEx(box, 0.2f, 8, 2.0f, COL_ACCENT);
        UiDrawCenteredText("Compiling and running tests...", SCREEN_W / 2, (int)box.y + 34, 22, RAYWHITE);
    }

    if (showSolution)
    {
        DrawSolutionOverlay();
        if (IsKeyPressed(KEY_ESCAPE)) showSolution = false;
    }
}