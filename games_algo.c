// Sort Sprint, Binary Hunt, Tree Architect, Path Finder, Big-O Showdown,
// Stack Attack, Code Tracer, Refactor Push.
#include "games_algo.h"
#include "theme.h"
#include "ui.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof((a)[0])))

static const AlgoGameInfo INFOS[] = {
    { GAME_SORT_SPRINT,    "Sort Sprint",      "Race to sort bars with bubble-style swaps before time runs out.", "Sorting" },
    { GAME_BINARY_HUNT,    "Binary Search Hunt","Guess the secret index using binary-search feedback.", "Searching" },
    { GAME_TREE_ARCHITECT, "Tree Architect",   "Place keys into a BST without breaking the order rules.", "Trees" },
    { GAME_PATH_FINDER,    "Path Finder",      "Carve a BFS path from start to goal on a blocked grid.", "Graphs" },
    { GAME_BIG_O,          "Big-O Showdown",   "Pick the correct complexity before the timer hits zero.", "Theory" },
    { GAME_STACK_ATTACK,   "Stack Attack",     "Push and pop to match a target sequence.", "Stacks" },
    { GAME_CODE_TRACER,    "Code Tracer",      "Predict the output of a short C/C++ snippet.", "Tracing" },
    { GAME_REFACTOR_PUSH,  "Refactor Push",    "Choose the cleaner refactor for a messy snippet.", "Style" }
};

int AlgoGameCount(void) { return ARRAY_SIZE(INFOS); }
const AlgoGameInfo *AlgoGameGet(int index)
{
    if (index < 0 || index >= AlgoGameCount()) return 0;
    return &INFOS[index];
}

static AlgoGameId active;
static unsigned seed;

static unsigned Rnd(void)
{
    seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
    if (seed == 0) seed = 1;
    return seed;
}

static bool BackBtn(void)
{
    return UiButton((Rectangle){ 30, 30, 120, 44 }, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE);
}

// ===================== Sort Sprint =====================
#define SS_N 8
static int ssArr[SS_N];
static int ssPick;
static float ssTime;
static int ssMoves;
static bool ssWon;

static void SSEnter(void)
{
    for (int i = 0; i < SS_N; i++) ssArr[i] = i + 1;
    for (int i = SS_N - 1; i > 0; i--)
    {
        int j = (int)(Rnd() % (unsigned)(i + 1));
        int t = ssArr[i]; ssArr[i] = ssArr[j]; ssArr[j] = t;
    }
    ssPick = -1; ssTime = 45.0f; ssMoves = 0; ssWon = false;
}

static bool SSSorted(void)
{
    for (int i = 0; i < SS_N - 1; i++) if (ssArr[i] > ssArr[i + 1]) return false;
    return true;
}

static bool SSRun(float dt)
{
    if (!ssWon) ssTime -= dt;
    UiDrawCenteredText("Sort Sprint", SCREEN_W / 2, 36, 34, RAYWHITE);
    char hud[64];
    snprintf(hud, sizeof(hud), "Time: %.1fs   Moves: %d   Swap two bars to sort ascending", ssTime > 0 ? ssTime : 0, ssMoves);
    UiDrawCenteredText(hud, SCREEN_W / 2, 90, 20, COL_MUTED);

    Vector2 mouse = GetMousePosition();
    float bw = 90, gap = 16, x0 = (SCREEN_W - SS_N * (bw + gap)) / 2;
    for (int i = 0; i < SS_N; i++)
    {
        float h = ssArr[i] * 36.0f;
        Rectangle r = { x0 + i * (bw + gap), 520 - h, bw, h };
        bool hov = CheckCollisionPointRec(mouse, r);
        Color c = (i == ssPick) ? COL_ACCENT : (hov ? COL_CYAN : COL_PANEL);
        DrawRectangleRounded(r, 0.12f, 6, c);
        char t[8]; snprintf(t, sizeof(t), "%d", ssArr[i]);
        UiDrawCenteredText(t, (int)(r.x + bw / 2), (int)(520 - h) - 28, 22, RAYWHITE);
        if (!ssWon && ssTime > 0 && hov && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if (ssPick < 0) ssPick = i;
            else if (ssPick == i) ssPick = -1;
            else
            {
                int tmp = ssArr[ssPick]; ssArr[ssPick] = ssArr[i]; ssArr[i] = tmp;
                ssPick = -1; ssMoves++;
                if (SSSorted()) { ssWon = true; UiToast("Sorted!"); }
            }
        }
    }

    if (ssWon) UiDrawCenteredText("Sorted! Great swaps.", SCREEN_W / 2, 580, 26, COL_DONE);
    else if (ssTime <= 0) UiDrawCenteredText("Time up — hit Restart.", SCREEN_W / 2, 580, 26, COL_ERROR);

    if (UiButton((Rectangle){ SCREEN_W / 2.0f - 120, 640, 240, 48 }, "Restart", true, 1.0f)) SSEnter();
    return BackBtn();
}

// ===================== Binary Search Hunt =====================
static int bhLo, bhHi, bhSecret, bhGuesses;
static bool bhWon, bhLost;
static char bhMsg[80];

static void BHEnter(void)
{
    bhLo = 1; bhHi = 100; bhSecret = (int)(Rnd() % 100) + 1;
    bhGuesses = 0; bhWon = bhLost = false;
    snprintf(bhMsg, sizeof(bhMsg), "Secret is in [%d, %d]. Pick the midpoint!", bhLo, bhHi);
}

static bool BHRun(float dt)
{
    (void)dt;
    UiDrawCenteredText("Binary Search Hunt", SCREEN_W / 2, 36, 34, RAYWHITE);
    UiDrawCenteredText("Think like binary search: always probe the middle of the live range.", SCREEN_W / 2, 90, 18, COL_MUTED);
    UiDrawCenteredText(bhMsg, SCREEN_W / 2, 140, 22, COL_CYAN);

    char st[64];
    snprintf(st, sizeof(st), "Guesses: %d / 7    Range: %d .. %d", bhGuesses, bhLo, bhHi);
    UiDrawCenteredText(st, SCREEN_W / 2, 190, 20, COL_MUTED);

    if (!bhWon && !bhLost)
    {
        int mid = (bhLo + bhHi) / 2;
        // Offer mid and a few distractors
        int opts[4] = { mid, bhLo, bhHi, (bhLo + mid) / 2 };
        if (opts[3] == opts[0]) opts[3] = mid + 1;
        for (int i = 0; i < 4; i++)
        {
            char label[32];
            snprintf(label, sizeof(label), "Probe %d", opts[i]);
            if (UiButton((Rectangle){ 240.0f + (i % 2) * 400.0f, 260.0f + (i / 2) * 90.0f, 360, 70 }, label, true, 1.0f))
            {
                bhGuesses++;
                int g = opts[i];
                if (g == bhSecret)
                {
                    bhWon = true;
                    snprintf(bhMsg, sizeof(bhMsg), "Found %d in %d probes!", bhSecret, bhGuesses);
                    UiToast("Found it!");
                }
                else if (g < bhSecret)
                {
                    if (g + 1 > bhLo) bhLo = g + 1;
                    snprintf(bhMsg, sizeof(bhMsg), "%d is too low. New range [%d, %d]", g, bhLo, bhHi);
                }
                else
                {
                    if (g - 1 < bhHi) bhHi = g - 1;
                    snprintf(bhMsg, sizeof(bhMsg), "%d is too high. New range [%d, %d]", g, bhLo, bhHi);
                }
                if (!bhWon && (bhGuesses >= 7 || bhLo > bhHi))
                {
                    bhLost = true;
                    snprintf(bhMsg, sizeof(bhMsg), "Out of probes. Secret was %d.", bhSecret);
                }
            }
        }
        UiDrawCenteredText(TextFormat("Hint: classic mid would be %d", mid), SCREEN_W / 2, 480, 20, COL_MUTED);
    }
    else
    {
        UiDrawCenteredText(bhWon ? "Optimal-ish hunting pays off." : "Binary search halves the space each time.", SCREEN_W / 2, 320, 24, bhWon ? COL_DONE : COL_ERROR);
        if (UiButton((Rectangle){ SCREEN_W / 2.0f - 120, 400, 240, 52 }, "Play again", true, 1.0f)) BHEnter();
    }
    return BackBtn();
}

// ===================== Tree Architect =====================
#define TA_N 7
static int taKeys[TA_N];
static int taPlaced[TA_N];
static int taCount;
static int taNext;
static bool taOk;

static void TAEnter(void)
{
    int pool[TA_N] = { 50, 30, 70, 20, 40, 60, 80 };
    for (int i = TA_N - 1; i > 0; i--)
    {
        int j = (int)(Rnd() % (unsigned)(i + 1));
        int t = pool[i]; pool[i] = pool[j]; pool[j] = t;
    }
    memcpy(taKeys, pool, sizeof(pool));
    memset(taPlaced, 0, sizeof(taPlaced));
    taCount = 0; taNext = 0; taOk = true;
}

static bool TAValidInsert(int key)
{
    // Validate against already placed keys as BST array built in insertion order —
    // check pairwise: simulate walk
    if (taCount == 0) return true;
    // Build simple check: for every placed key, relative order with new key must be
    // consistent with a BST insert path — use recursive structure stored in order of placement
    // Easier rule for game: player picks position slots left/right of root visually.
    // Here: accept any key; scoring checks final inorder equals sorted.
    (void)key;
    return true;
}

static int TACmp(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }

static bool TAIsBstShape(void)
{
    // After all placed in insertion order into complete tree slots 0..n-1 by successive
    // BST inserts into an array heap layout used in explore — verify inorder sorted
    // For this game: placed order is insertion sequence into empty BST; check each insert
    // would land without violating by reconstructing.
    int vals[TA_N];
    for (int i = 0; i < taCount; i++) vals[i] = taPlaced[i];
    // Reconstruct BST in a linked-style recursive array of left/right children
    typedef struct { int key; int left; int right; } N;
    N nodes[TA_N];
    int used = 0;
    if (taCount == 0) return true;
    nodes[0].key = vals[0]; nodes[0].left = nodes[0].right = -1; used = 1;
    for (int i = 1; i < taCount; i++)
    {
        int cur = 0;
        for (;;)
        {
            if (vals[i] == nodes[cur].key) return false;
            if (vals[i] < nodes[cur].key)
            {
                if (nodes[cur].left < 0)
                {
                    nodes[cur].left = used;
                    nodes[used].key = vals[i];
                    nodes[used].left = nodes[used].right = -1;
                    used++;
                    break;
                }
                cur = nodes[cur].left;
            }
            else
            {
                if (nodes[cur].right < 0)
                {
                    nodes[cur].right = used;
                    nodes[used].key = vals[i];
                    nodes[used].left = nodes[used].right = -1;
                    used++;
                    break;
                }
                cur = nodes[cur].right;
            }
        }
    }
    // inorder
    int stack[TA_N], top = 0, out[TA_N], oc = 0, c = 0;
    while (c >= 0 || top)
    {
        while (c >= 0) { stack[top++] = c; c = nodes[c].left; }
        c = stack[--top];
        out[oc++] = nodes[c].key;
        c = nodes[c].right;
    }
    for (int i = 1; i < oc; i++) if (out[i - 1] >= out[i]) return false;
    return true;
}

static bool TARun(float dt)
{
    (void)dt;
    UiDrawCenteredText("Tree Architect", SCREEN_W / 2, 36, 34, RAYWHITE);
    UiDrawCenteredText("Insert the next key into the BST. Keep left < parent < right.", SCREEN_W / 2, 90, 18, COL_MUTED);

    if (taNext < TA_N)
    {
        char n[48];
        snprintf(n, sizeof(n), "Next key to insert: %d", taKeys[taNext]);
        UiDrawCenteredText(n, SCREEN_W / 2, 140, 26, COL_ACCENT);
        if (UiButtonPrimary((Rectangle){ SCREEN_W / 2.0f - 140, 200, 280, 56 }, "Insert into BST", true, 1.0f))
        {
            if (TAValidInsert(taKeys[taNext]))
            {
                taPlaced[taCount++] = taKeys[taNext++];
                taOk = TAIsBstShape();
                if (!taOk) UiToast("Order broken!");
                else if (taNext >= TA_N) UiToast("Tree complete!");
            }
        }
    }
    else
    {
        UiDrawCenteredText(taOk ? "Beautiful BST — inorder is sorted." : "Something went wrong.", SCREEN_W / 2, 200, 26, taOk ? COL_DONE : COL_ERROR);
        if (UiButton((Rectangle){ SCREEN_W / 2.0f - 120, 260, 240, 48 }, "Rebuild", true, 1.0f)) TAEnter();
    }

    // draw placed keys
    for (int i = 0; i < taCount; i++)
    {
        float x = 120.0f + i * 150.0f;
        DrawCircleV((Vector2){ x, 420 }, 30, COL_CYAN);
        char t[8]; snprintf(t, sizeof(t), "%d", taPlaced[i]);
        UiDrawCenteredText(t, (int)x, 410, 20, COL_BG);
        if (i + 1 < taCount)
            DrawLineEx((Vector2){ x + 30, 420 }, (Vector2){ x + 120, 420 }, 2, COL_MUTED);
    }
    UiDrawCenteredText("Insertion sequence (root first)", SCREEN_W / 2, 480, 18, COL_MUTED);
    return BackBtn();
}

// ===================== Path Finder =====================
#define PF 8
static int pfGrid[PF * PF];
static int pfSel;
static bool pfWon;

static void PFEnter(void)
{
    memset(pfGrid, 0, sizeof(pfGrid));
    for (int i = 0; i < PF * PF; i++) if (Rnd() % 4 == 0) pfGrid[i] = -1;
    pfGrid[0] = 0; pfGrid[PF * PF - 1] = 0;
    pfSel = -1; pfWon = false;
}

static bool PFReachable(void)
{
    int q[PF * PF], h = 0, t = 0, seen[PF * PF] = {0};
    q[t++] = 0; seen[0] = 1;
    const int dr[4] = {-1,1,0,0}, dc[4]={0,0,-1,1};
    while (h < t)
    {
        int u = q[h++];
        if (u == PF * PF - 1) return true;
        int r = u / PF, c = u % PF;
        for (int k = 0; k < 4; k++)
        {
            int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nc < 0 || nr >= PF || nc >= PF) continue;
            int v = nr * PF + nc;
            if (seen[v] || pfGrid[v] == -1) continue;
            seen[v] = 1; q[t++] = v;
        }
    }
    return false;
}

static bool PFRun(float dt)
{
    (void)dt;
    UiDrawCenteredText("Path Finder", SCREEN_W / 2, 30, 34, RAYWHITE);
    UiDrawCenteredText("Toggle walls (avoid start/goal). Path must exist via BFS.", SCREEN_W / 2, 78, 18, COL_MUTED);

    float size = 56, gap = 4;
    float ox = (SCREEN_W - PF * (size + gap)) / 2, oy = 120;
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < PF * PF; i++)
    {
        int r = i / PF, c = i % PF;
        Rectangle cell = { ox + c * (size + gap), oy + r * (size + gap), size, size };
        Color fill = COL_PANEL;
        if (i == 0) fill = COL_DONE;
        else if (i == PF * PF - 1) fill = COL_ACCENT;
        else if (pfGrid[i] == -1) fill = COL_LOCKED;
        DrawRectangleRounded(cell, 0.12f, 4, fill);
        if (CheckCollisionPointRec(mouse, cell) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if (i != 0 && i != PF * PF - 1)
                pfGrid[i] = (pfGrid[i] == -1) ? 0 : -1;
        }
    }

    if (UiButtonPrimary((Rectangle){ SCREEN_W / 2.0f - 160, 620, 320, 52 }, "Check BFS path", true, 1.0f))
    {
        pfWon = PFReachable();
        UiToast(pfWon ? "Path exists!" : "Blocked — open a route");
    }
    if (pfWon) UiDrawCenteredText("Reachable — that is BFS connectivity.", SCREEN_W / 2, 580, 22, COL_DONE);
    if (UiButton((Rectangle){ 40, 620, 160, 52 }, "New map", true, 1.0f)) PFEnter();
    return BackBtn();
}

// ===================== Big-O Showdown =====================
typedef struct { const char *q; const char *opts[4]; int correct; } BigQ;
static const BigQ BIGQ[] = {
    { "Binary search on a sorted array?", { "O(n)", "O(log n)", "O(n^2)", "O(1)" }, 1 },
    { "Nested loop i=0..n, j=0..n?", { "O(n)", "O(n log n)", "O(n^2)", "O(2^n)" }, 2 },
    { "Hash map average lookup?", { "O(1)", "O(log n)", "O(n)", "O(n log n)" }, 0 },
    { "Merge sort typical time?", { "O(n)", "O(n log n)", "O(n^2)", "O(log n)" }, 1 },
    { "Walking a linked list of n nodes?", { "O(1)", "O(log n)", "O(n)", "O(n^2)" }, 2 },
    { "BFS on graph V vertices, E edges?", { "O(V+E)", "O(V^2)", "O(E log V)", "O(1)" }, 0 },
    { "Access array index i?", { "O(1)", "O(n)", "O(log n)", "O(i)" }, 0 },
    { "Naive recursive fibonacci?", { "O(n)", "O(n log n)", "O(2^n)", "O(1)" }, 2 }
};
static int boIndex, boScore, boLives;
static float boTime;
static bool boDone;

static void BOEnter(void)
{
    boIndex = (int)(Rnd() % ARRAY_SIZE(BIGQ));
    boScore = 0; boLives = 3; boTime = 12.0f; boDone = false;
}

static void BONext(void)
{
    boIndex = (int)(Rnd() % ARRAY_SIZE(BIGQ));
    boTime = 12.0f;
}

static bool BORun(float dt)
{
    UiDrawCenteredText("Big-O Showdown", SCREEN_W / 2, 36, 34, RAYWHITE);
    if (boDone)
    {
        char m[64]; snprintf(m, sizeof(m), "Score: %d", boScore);
        UiDrawCenteredText(m, SCREEN_W / 2, 200, 36, COL_ACCENT);
        if (UiButton((Rectangle){ SCREEN_W / 2.0f - 120, 300, 240, 52 }, "Again", true, 1.0f)) BOEnter();
        return BackBtn();
    }

    boTime -= dt;
    if (boTime <= 0)
    {
        boLives--;
        UiToast("Too slow!");
        if (boLives <= 0) boDone = true;
        else BONext();
    }

    const BigQ *q = &BIGQ[boIndex];
    char hud[64];
    snprintf(hud, sizeof(hud), "Score %d    Lives %d    Time %.1f", boScore, boLives, boTime);
    UiDrawCenteredText(hud, SCREEN_W / 2, 90, 20, COL_MUTED);
    UiDrawWrappedText(q->q, (Rectangle){ 160, 150, 960, 80 }, 28, RAYWHITE);

    for (int i = 0; i < 4; i++)
    {
        if (UiButton((Rectangle){ 200.0f + (i % 2) * 440.0f, 280.0f + (i / 2) * 100.0f, 400, 80 }, q->opts[i], true, 1.0f))
        {
            if (i == q->correct) { boScore += 10; UiToast("Correct!"); BONext(); }
            else
            {
                boLives--;
                UiToast("Nope");
                if (boLives <= 0) boDone = true;
                else BONext();
            }
        }
    }
    return BackBtn();
}

// ===================== Stack Attack =====================
static int saStack[12], saTop;
static int saTarget[6], saTLen;
static int saInput[6], saInLen;
static bool saWon;

static void SAEnter(void)
{
    saTop = 0; saInLen = 0; saWon = false;
    saTLen = 4;
    for (int i = 0; i < saTLen; i++) saTarget[i] = (int)(Rnd() % 9) + 1;
}

static bool SARun(float dt)
{
    (void)dt;
    UiDrawCenteredText("Stack Attack", SCREEN_W / 2, 36, 34, RAYWHITE);
    UiDrawCenteredText("Push numbers, then pop them into the output to match the target.", SCREEN_W / 2, 90, 18, COL_MUTED);

    char target[64] = "Target output: ";
    for (int i = 0; i < saTLen; i++)
    {
        char b[8]; snprintf(b, sizeof(b), "%d ", saTarget[i]);
        strncat(target, b, sizeof(target) - strlen(target) - 1);
    }
    UiDrawCenteredText(target, SCREEN_W / 2, 140, 24, COL_ACCENT);

    char out[64] = "Your output: ";
    for (int i = 0; i < saInLen; i++)
    {
        char b[8]; snprintf(b, sizeof(b), "%d ", saInput[i]);
        strncat(out, b, sizeof(out) - strlen(out) - 1);
    }
    UiDrawCenteredText(out, SCREEN_W / 2, 190, 22, COL_CYAN);

    // stack draw
    for (int i = 0; i < saTop; i++)
    {
        Rectangle r = { SCREEN_W / 2.0f - 60, 520.0f - i * 42.0f, 120, 38 };
        DrawRectangleRounded(r, 0.15f, 4, COL_PANEL);
        char t[8]; snprintf(t, sizeof(t), "%d", saStack[i]);
        UiDrawCenteredText(t, SCREEN_W / 2, (int)r.y + 8, 20, RAYWHITE);
    }

    for (int n = 1; n <= 9; n++)
    {
        char lab[4]; snprintf(lab, sizeof(lab), "%d", n);
        if (UiButton((Rectangle){ 80.0f + (n - 1) * 120.0f, 560, 100, 44 }, lab, saTop < 12 && !saWon, 1.0f))
            saStack[saTop++] = n;
    }
    if (UiButton((Rectangle){ 400, 620, 200, 48 }, "Pop -> output", saTop > 0 && saInLen < saTLen && !saWon, 1.0f))
    {
        saInput[saInLen++] = saStack[--saTop];
        if (saInLen == saTLen)
        {
            saWon = true;
            for (int i = 0; i < saTLen; i++) if (saInput[i] != saTarget[i]) saWon = false;
            UiToast(saWon ? "Sequence matched!" : "Not the target — reset");
        }
    }
    if (UiButton((Rectangle){ 640, 620, 200, 48 }, "Reset", true, 1.0f)) SAEnter();
    if (saWon) UiDrawCenteredText("Stack discipline wins.", SCREEN_W / 2, 520, 22, COL_DONE);
    return BackBtn();
}

// ===================== Code Tracer =====================
typedef struct { const char *code; const char *opts[4]; int correct; const char *why; } TraceQ;
static const TraceQ TRACES[] = {
    { "int x = 3;\nx += 2;\ncout << x;", { "3", "5", "2", "6" }, 1, "3+2 is 5." },
    { "int a[3] = {1,2,3};\ncout << a[1];", { "1", "2", "3", "0" }, 1, "Index 1 is the second element." },
    { "int n = 5;\ncout << n % 2;", { "2", "1", "0", "5" }, 1, "5 divided by 2 leaves remainder 1." },
    { "int i = 0;\nwhile (i < 3) i++;\ncout << i;", { "2", "3", "4", "0" }, 1, "Loop stops when i becomes 3." },
    { "int *p = nullptr;\ncout << (p == nullptr);", { "0", "1", "crash", "2" }, 1, "Comparison yields true (1)." },
    { "int s = 0;\nfor (int i=1;i<=3;i++) s+=i;\ncout << s;", { "3", "6", "9", "0" }, 1, "1+2+3 = 6." }
};
static int trIndex, trChoice;
static bool trAnswered;

static void TREnter(void)
{
    trIndex = (int)(Rnd() % ARRAY_SIZE(TRACES));
    trChoice = -1; trAnswered = false;
}

static bool TRRun(float dt)
{
    (void)dt;
    const TraceQ *q = &TRACES[trIndex];
    UiDrawCenteredText("Code Tracer", SCREEN_W / 2, 30, 34, RAYWHITE);
    UiDrawCenteredText("What does this print?", SCREEN_W / 2, 78, 20, COL_MUTED);
    UiDrawCodeBlock(q->code, (Rectangle){ 200, 110, 880, 200 });

    for (int i = 0; i < 4; i++)
    {
        bool enabled = !trAnswered;
        if (UiButton((Rectangle){ 220.0f + (i % 2) * 420.0f, 350.0f + (i / 2) * 80.0f, 380, 64 }, q->opts[i], enabled, 1.0f))
        {
            trChoice = i; trAnswered = true;
            if (i == q->correct) UiToast("Correct trace!");
        }
    }
    if (trAnswered)
    {
        bool ok = trChoice == q->correct;
        UiDrawCenteredText(ok ? "Correct!" : "Not quite.", SCREEN_W / 2, 530, 24, ok ? COL_DONE : COL_ERROR);
        UiDrawWrappedText(q->why, (Rectangle){ 200, 570, 880, 40 }, 18, COL_MUTED);
        if (UiButton((Rectangle){ SCREEN_W / 2.0f - 120, 640, 240, 48 }, "Next snippet", true, 1.0f)) TREnter();
    }
    return BackBtn();
}

// ===================== Refactor Push =====================
typedef struct { const char *bad; const char *opts[3]; int correct; const char *why; } RefQ;
static const RefQ REFS[] = {
    { "if (x = 5) { ... }",
      { "Keep it — assignment in if is fine", "Use if (x == 5)", "Use if (x === 5)" }, 1,
      "Comparison needs ==. A single = assigns." },
    { "for (int i=0; i<=n; i++) sum += a[i];  // a has n elems",
      { "Use i < n", "Use i <= n+1", "Leave it" }, 0,
      "Valid indexes are 0..n-1." },
    { "int *p = new int[10]; ... delete p;",
      { "delete[] p;", "free(p);", "p = 0 only" }, 0,
      "Memory from new[] needs delete[]." },
    { "string s; cin >> s; // want full line",
      { "Keep cin >>", "Use std::getline(cin, s)", "Use scanf only" }, 1,
      "getline reads an entire line including spaces." },
    { "Heavy copy: void f(vector<int> v)",
      { "Pass by value always", "void f(const vector<int>& v)", "Use raw array only" }, 1,
      "Const reference avoids copying large containers." }
};
static int rfIndex, rfChoice;
static bool rfDone;

static void RFEnter(void)
{
    rfIndex = (int)(Rnd() % ARRAY_SIZE(REFS));
    rfChoice = -1; rfDone = false;
}

static bool RFRun(float dt)
{
    (void)dt;
    const RefQ *q = &REFS[rfIndex];
    UiDrawCenteredText("Refactor Push", SCREEN_W / 2, 30, 34, RAYWHITE);
    UiDrawCenteredText("Teammate-approved: pick the cleaner, safer change.", SCREEN_W / 2, 78, 18, COL_MUTED);
    DrawRectangleRounded((Rectangle){ 140, 120, 1000, 120 }, 0.08f, 8, COL_PANEL);
    UiDrawWrappedText(q->bad, (Rectangle){ 170, 150, 940, 70 }, 24, COL_ERROR);

    for (int i = 0; i < 3; i++)
    {
        if (UiButton((Rectangle){ 190, 280.0f + i * 90.0f, 900, 70 }, q->opts[i], !rfDone, 1.0f))
        {
            rfChoice = i; rfDone = true;
            UiToast(i == q->correct ? "Solid refactor" : "Try another angle");
        }
    }
    if (rfDone)
    {
        UiDrawWrappedText(q->why, (Rectangle){ 190, 560, 900, 50 }, 20, COL_MUTED);
        if (UiButton((Rectangle){ SCREEN_W / 2.0f - 120, 640, 240, 48 }, "Next", true, 1.0f)) RFEnter();
    }
    return BackBtn();
}

// ===================== API =====================
void AlgoGameEnter(AlgoGameId id)
{
    active = id;
    if (seed == 0) seed = (unsigned)time(NULL) | 1u;
    switch (id)
    {
    case GAME_SORT_SPRINT:    SSEnter(); break;
    case GAME_BINARY_HUNT:    BHEnter(); break;
    case GAME_TREE_ARCHITECT: TAEnter(); break;
    case GAME_PATH_FINDER:    PFEnter(); break;
    case GAME_BIG_O:          BOEnter(); break;
    case GAME_STACK_ATTACK:   SAEnter(); break;
    case GAME_CODE_TRACER:    TREnter(); break;
    case GAME_REFACTOR_PUSH:  RFEnter(); break;
    default: break;
    }
}

bool AlgoGameRun(AppState *app, float dt)
{
    (void)app;
    switch (active)
    {
    case GAME_SORT_SPRINT:    return SSRun(dt);
    case GAME_BINARY_HUNT:    return BHRun(dt);
    case GAME_TREE_ARCHITECT: return TARun(dt);
    case GAME_PATH_FINDER:    return PFRun(dt);
    case GAME_BIG_O:          return BORun(dt);
    case GAME_STACK_ATTACK:   return SARun(dt);
    case GAME_CODE_TRACER:    return TRRun(dt);
    case GAME_REFACTOR_PUSH:  return RFRun(dt);
    default: return true;
    }
}
