// Puzzle games: Code Order, Bug Hunt and Match Up.
#include "screens.h"
#include "theme.h"
#include "ui.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

// ===========================================================================
// Puzzle data and logic (no raylib calls in this part)
// ===========================================================================
#define PUZZLE_MAX_LINES 8
#define PAIR_COUNT       6
#define MATCH_CARDS      (PAIR_COUNT * 2)

typedef struct {
    const char *title;
    const char *goal;
    int lineCount;
    const char *lines[PUZZLE_MAX_LINES];   // in the correct order
} OrderPuzzle;

typedef struct {
    const char *title;
    const char *prompt;
    int lineCount;
    const char *lines[PUZZLE_MAX_LINES];
    int bugLine;                 // index of the buggy line
    const char *explanation;
} BugPuzzle;

typedef struct {
    const char *title;
    const char *terms[PAIR_COUNT];
    const char *meanings[PAIR_COUNT];
} PairSet;

typedef struct {
    int card[MATCH_CARDS];       // pair * 2 + side (0 = term, 1 = meaning)
    bool matched[MATCH_CARDS];
    int first;                   // flipped card positions, or -1
    int second;
    int moves;
    int pairsFound;
} MatchBoard;

#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof((a)[0])))

// ---------------------------------------------------------------------------
// Random numbers (xorshift32). The caller owns the seed.
// ---------------------------------------------------------------------------
static unsigned int NextRandom(unsigned int *seed)
{
    unsigned int x = *seed;
    if (x == 0) x = 2463534242u;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *seed = x;
    return x;
}

static void PuzzleShuffle(int *perm, int n, unsigned int *seed)
{
    for (int i = 0; i < n; i++) perm[i] = i;

    for (int i = n - 1; i > 0; i--)
    {
        int j = (int)(NextRandom(seed) % (unsigned int)(i + 1));
        int tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }
}

// ---------------------------------------------------------------------------
// Code Order
// ---------------------------------------------------------------------------
static const OrderPuzzle ORDER_PUZZLES[] = {
    { "Leap Year Check",
      "Build a function that tells you if a year is a leap year.",
      5,
      { "bool isLeap(int year) {",
        "    if (year % 400 == 0) return true;",
        "    if (year % 100 == 0) return false;",
        "    return year % 4 == 0;",
        "}" } },

    { "Heap Array",
      "Allocate an array on the heap, use it, then free it safely.",
      5,
      { "int *p = new int[3];",
        "p[0] = 10;",
        "cout << p[0];",
        "delete[] p;",
        "p = nullptr;" } },

    { "Push to Front",
      "Add a node to the front of a linked list, then print the new front value.",
      4,
      { "Node *n = new Node(7);",
        "n->next = head;",
        "head = n;",
        "cout << head->value;" } },

    { "Reverse a String",
      "Reverse a string in place by swapping characters from both ends.",
      5,
      { "int lo = 0, hi = len - 1;",
        "while (lo < hi) {",
        "    swap(s[lo], s[hi]);",
        "    lo++; hi--;",
        "}" } },

    { "Sum an Array",
      "Add up every value in an array and print the total.",
      5,
      { "int sum = 0;",
        "for (int i = 0; i < n; i++) {",
        "    sum += arr[i];",
        "}",
        "cout << sum;" } },

    { "Reverse a Number",
      "Reverse the digits of a number, for example 123 becomes 321.",
      6,
      { "int reversed = 0;",
        "while (n != 0) {",
        "    reversed = reversed * 10 + n % 10;",
        "    n /= 10;",
        "}",
        "return reversed;" } },

    { "Prime Check",
      "Test whether a number is prime by trying every divisor up to its square root.",
      5,
      { "bool isPrime(int n) {",
        "    for (int i = 2; i * i <= n; i++)",
        "        if (n % i == 0) return false;",
        "    return n > 1;",
        "}" } },

    { "Right-Angled Triangle",
      "Print a triangle of A's: one A on the first row, two on the second, and so on.",
      5,
      { "for (int row = 1; row <= 4; row++) {",
        "    for (int col = 1; col <= row; col++)",
        "        printf(\"A\");",
        "    printf(\"\\n\");",
        "}" } },

    { "Rotate Left",
      "Move every element one place to the left and put the first element at the end.",
      4,
      { "int first = arr[0];",
        "for (int i = 0; i < size - 1; i++)",
        "    arr[i] = arr[i + 1];",
        "arr[size - 1] = first;" } }
};

static int OrderPuzzleCount(void)
{
    return ARRAY_SIZE(ORDER_PUZZLES);
}

static const OrderPuzzle *OrderPuzzleGet(int index)
{
    if (index < 0 || index >= OrderPuzzleCount()) return 0;
    return &ORDER_PUZZLES[index];
}

static bool OrderLineCorrect(const OrderPuzzle *puzzle, const int *perm, int row)
{
    if (row < 0 || row >= puzzle->lineCount) return false;
    // Compare the text, so two identical lines can be swapped freely
    return strcmp(puzzle->lines[perm[row]], puzzle->lines[row]) == 0;
}

static bool OrderPuzzleSolved(const OrderPuzzle *puzzle, const int *perm)
{
    for (int i = 0; i < puzzle->lineCount; i++)
    {
        if (!OrderLineCorrect(puzzle, perm, i)) return false;
    }
    return true;
}

static void OrderPuzzleShuffle(const OrderPuzzle *puzzle, int *perm, unsigned int *seed)
{
    for (int attempt = 0; attempt < 100; attempt++)
    {
        PuzzleShuffle(perm, puzzle->lineCount, seed);
        if (!OrderPuzzleSolved(puzzle, perm)) return;
    }

    // Practically unreachable; make sure the player never starts on a solved board
    if (puzzle->lineCount >= 2)
    {
        int tmp = perm[0];
        perm[0] = perm[puzzle->lineCount - 1];
        perm[puzzle->lineCount - 1] = tmp;
    }
}

// ---------------------------------------------------------------------------
// Bug Hunt
// ---------------------------------------------------------------------------
static const BugPuzzle BUG_PUZZLES[] = {
    { "Off by One",
      "This loop should add up all five numbers. Which line is the bug?",
      5,
      { "int arr[5] = {1, 2, 3, 4, 5};",
        "int sum = 0;",
        "for (int i = 0; i <= 5; i++)",
        "    sum += arr[i];",
        "cout << sum;" },
      2,
      "Valid indexes are 0 to 4. With i <= 5 the loop also reads arr[5], which is past the end of the array. Use i < 5." },

    { "Use After Delete",
      "This program frees some memory and then keeps using it. Which line is the bug?",
      4,
      { "int *p = new int(10);",
        "*p += 5;",
        "delete p;",
        "cout << *p;" },
      3,
      "After delete the memory no longer belongs to your program, so reading *p is undefined behaviour. Set p = nullptr after deleting and do not use it again." },

    { "One Equals Sign",
      "This should only print Perfect when score is 5. Which line is the bug?",
      5,
      { "int score = 3;",
        "if (score = 5)",
        "    cout << \"Perfect\";",
        "else",
        "    cout << \"Try again\";" },
      1,
      "A single = assigns 5 to score, so the condition is always true. Comparing two values needs ==." },

    { "Wrong Delete",
      "An array is allocated on the heap and then freed. Which line is the bug?",
      4,
      { "int *a = new int[10];",
        "a[0] = 1;",
        "cout << a[0];",
        "delete a;" },
      3,
      "Memory from new[] must be released with delete[]. Using plain delete on an array is undefined behaviour." },

    { "Pointing at Nothing",
      "This program crashes. Which line is the first one to go wrong?",
      3,
      { "int *ptr = nullptr;",
        "*ptr = 42;",
        "cout << \"Done\";" },
      1,
      "ptr is nullptr, so it points to nothing and writing through it crashes. Make it point at a real int first, for example with new int." },

    { "Typo in a Name",
      "This function should reverse a number, but it will not compile. Which line is the bug?",
      6,
      { "int reversed = 0;",
        "while (testNumber != 0) {",
        "    int digit = testNumber % 10;",
        "    reversed = reversed * 10 + digt;",
        "    testNumber /= 10;",
        "}" },
      3,
      "The variable is called digit, but this line uses digt, which was never declared, so the program will not compile. Check the spelling of your names." },

    { "Printing a Number",
      "This should print the year when it is a leap year. Which line is the bug?",
      5,
      { "int year = 2016;",
        "if (year % 4 == 0)",
        "    printf(year);",
        "else",
        "    printf(\"Not a leap year\");" },
      2,
      "printf needs a format string first, then the values to print. The correct call is printf(\"%d\", year);" },

    { "Missing Bracket",
      "This leap year function will not compile. Which line is the bug?",
      5,
      { "bool isLeap(int year) {",
        "    if (year % 400 == 0) return true;",
        "    if (year % 100 == 0 return false;",
        "    return year % 4 == 0;",
        "}" },
      2,
      "The condition in the second if is missing its closing bracket after the 0. Every ( needs a matching )." },

    { "Everything Is Divisible",
      "This prime check says no for every number from 2 up. Which line is the bug?",
      5,
      { "bool isPrime(int n) {",
        "    for (int i = 2; i <= n; i++)",
        "        if (n % i == 0) return false;",
        "    return true;",
        "}" },
      1,
      "When i reaches n, n % i is 0, so every number looks divisible by itself. The loop must stop before n: use i < n, or i * i <= n." }
};

static int BugPuzzleCount(void)
{
    return ARRAY_SIZE(BUG_PUZZLES);
}

static const BugPuzzle *BugPuzzleGet(int index)
{
    if (index < 0 || index >= BugPuzzleCount()) return 0;
    return &BUG_PUZZLES[index];
}

// ---------------------------------------------------------------------------
// Match Up
// ---------------------------------------------------------------------------
static const PairSet PAIR_SETS[] = {
    { "Programming Basics",
      { "int", "double", "char", "bool", "for", "%" },
      { "whole number", "decimal number", "single character", "true or false", "loop with a counter", "remainder after division" } },

    { "Pointers and Memory",
      { "&x", "*p", "nullptr", "new", "delete", "p + 1" },
      { "address of x", "value p points to", "points to nothing", "allocates on the heap", "frees heap memory", "next array element" } },

    { "Classes and the STL",
      { "this", "constructor", "vector", "map", "template", "struct" },
      { "pointer to the current object", "runs when an object is made", "resizable array", "key and value pairs", "one function for many types", "groups related data" } },

    { "Lab Array Operations",
      { "find", "insert", "delete", "reverse", "frequency", "rotate left" },
      { "index of a value, or -1", "shift elements right", "shift elements left", "swap from both ends", "how often a value appears", "first element goes to the end" } }
};

static int PairSetCount(void)
{
    return ARRAY_SIZE(PAIR_SETS);
}

static const PairSet *PairSetGet(int index)
{
    if (index < 0 || index >= PairSetCount()) return 0;
    return &PAIR_SETS[index];
}

static void MatchInit(MatchBoard *board, unsigned int *seed)
{
    memset(board, 0, sizeof(*board));
    board->first = -1;
    board->second = -1;
    PuzzleShuffle(board->card, MATCH_CARDS, seed);   // card ids 0..11 in random places
}

static int MatchPairOf(const MatchBoard *board, int position)
{
    return board->card[position] / 2;
}

static int MatchSideOf(const MatchBoard *board, int position)
{
    return board->card[position] % 2;
}

static bool MatchPending(const MatchBoard *board)
{
    return board->second >= 0;
}

static bool MatchFlip(MatchBoard *board, int position)
{
    if (position < 0 || position >= MATCH_CARDS) return false;
    if (board->matched[position]) return false;
    if (MatchPending(board)) return false;
    if (position == board->first) return false;

    if (board->first < 0)
    {
        board->first = position;
    }
    else
    {
        board->second = position;
        board->moves++;
    }
    return true;
}

static bool MatchResolve(MatchBoard *board)
{
    if (!MatchPending(board)) return false;

    bool isPair = MatchPairOf(board, board->first) == MatchPairOf(board, board->second);
    if (isPair)
    {
        board->matched[board->first] = true;
        board->matched[board->second] = true;
        board->pairsFound++;
    }

    board->first = -1;
    board->second = -1;
    return isPair;
}

static bool MatchWon(const MatchBoard *board)
{
    return board->pairsFound >= PAIR_COUNT;
}

// ===========================================================================
// Screen
// ===========================================================================

typedef enum { MODE_HUB, MODE_ORDER, MODE_BUG, MODE_MATCH } Mode;

#define ROW_X     190.0f
#define ROW_W     900.0f
#define ROW_H     46.0f
#define ROW_STEP  54.0f
#define ROW_TOP   170.0f

#define MATCH_COLS   4
#define MATCH_CARD_W 240.0f
#define MATCH_CARD_H 100.0f
#define MATCH_GAP    20.0f
#define RESOLVE_TIME 0.9f

static const Color CODE_BG    = { 10, 12, 24, 255 };
static const Color CODE_TEXT  = { 200, 230, 200, 255 };
static const Color RIGHT_FILL = { 30, 80, 55, 255 };
static const Color WRONG_FILL = { 90, 35, 45, 255 };

static Mode mode;
static unsigned int seed;
static int solvedCount;          // puzzles solved since the app started

// Code Order
static int orderIndex;
static int orderPerm[PUZZLE_MAX_LINES];
static int picked;               // row lifted by the first click, or -1
static bool orderChecked;
static bool orderSolved;

// Bug Hunt
static int bugIndex;
static int bugChosen;            // row the player clicked, or -1

// Match Up
static int setIndex;
static MatchBoard board;
static float resolveTimer;

// ---------------------------------------------------------------------------
// Starting each game
// ---------------------------------------------------------------------------
static void StartOrder(int index)
{
    mode = MODE_ORDER;
    orderIndex = index % OrderPuzzleCount();
    OrderPuzzleShuffle(OrderPuzzleGet(orderIndex), orderPerm, &seed);
    picked = -1;
    orderChecked = false;
    orderSolved = false;
}

static void StartBug(int index)
{
    mode = MODE_BUG;
    bugIndex = index % BugPuzzleCount();
    bugChosen = -1;
}

static void StartMatch(int index)
{
    mode = MODE_MATCH;
    setIndex = index % PairSetCount();
    MatchInit(&board, &seed);
    resolveTimer = 0.0f;
}

void ScreenPuzzlesEnter(AppState *app)
{
    (void)app;
    if (seed == 0) seed = (unsigned int)time(NULL) | 1u;
    mode = MODE_HUB;
}

// ---------------------------------------------------------------------------
// Shared bits
// ---------------------------------------------------------------------------
static bool BackPressed(void)
{
    return UiButton((Rectangle){ 30, 30, 120, 44 }, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE);
}

static void DrawHeader(const char *kind, const char *title, const char *subtitle)
{
    char line[128];
    snprintf(line, sizeof(line), "%s: %s", kind, title);
    UiDrawCenteredText(line, SCREEN_W / 2, 36, 34, RAYWHITE);
    UiDrawCenteredText(subtitle, SCREEN_W / 2, 90, 20, COL_MUTED);
}

// A row of code, used by Code Order and Bug Hunt
static void DrawCodeRow(Rectangle r, int number, const char *text, Color fill, Color border)
{
    DrawRectangleRounded(r, 0.2f, 8, fill);
    DrawRectangleRoundedLinesEx(r, 0.2f, 8, 2.0f, border);

    char num[8];
    snprintf(num, sizeof(num), "%d", number);
    UiDrawText(num, (int)r.x + 18, (int)r.y + 13, 20, COL_MUTED);
    UiDrawText(text, (int)r.x + 64, (int)r.y + 12, 22, CODE_TEXT);
}

// ---------------------------------------------------------------------------
// Hub
// ---------------------------------------------------------------------------
static void RunHub(AppState *app)
{
    static const char *titles[3] = { "Code Order", "Bug Hunt", "Match Up" };
    static const char *blurbs[3] = {
        "The lines of a program have been shuffled. Swap them back into the right order.",
        "One line in each snippet is broken. Click the line that contains the bug.",
        "Flip cards to pair each term with its meaning. Fewer moves is better."
    };
    const Color colors[3] = { COL_ACCENT, COL_CYAN, COL_DONE };
    const int counts[3] = { OrderPuzzleCount(), BugPuzzleCount(), PairSetCount() };

    UiDrawCenteredText("Puzzle Games", SCREEN_W / 2, 50, 44, RAYWHITE);
    UiDrawCenteredText("Put your C and C++ knowledge to work", SCREEN_W / 2, 106, 20, COL_MUTED);

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < 3; i++)
    {
        Rectangle r = { 100.0f + i * 370.0f, 180, 340, 340 };
        bool hovered = CheckCollisionPointRec(mouse, r);

        DrawRectangleRounded(r, 0.08f, 8, COL_PANEL);
        DrawRectangleRoundedLinesEx(r, 0.08f, 8, 3.0f, hovered ? colors[i] : Fade(COL_MUTED, 0.4f));

        Vector2 badge = { r.x + r.width / 2, r.y + 70 };
        DrawCircleV(badge, 38, colors[i]);
        char num[2] = { (char)('1' + i), '\0' };
        UiDrawText(num, (int)badge.x - UiMeasureText(num, 40) / 2, (int)badge.y - 20, 40, COL_BG);

        UiDrawCenteredText(titles[i], (int)(r.x + r.width / 2), (int)r.y + 130, 30, RAYWHITE);
        UiDrawWrappedText(blurbs[i], (Rectangle){ r.x + 28, r.y + 180, r.width - 56, 100 }, 20, COL_MUTED);

        char count[32];
        snprintf(count, sizeof(count), "%d %s", counts[i], i == 2 ? "sets" : "puzzles");
        UiDrawCenteredText(count, (int)(r.x + r.width / 2), (int)r.y + 296, 18, colors[i]);

        if (hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if (i == 0)      StartOrder(0);
            else if (i == 1) StartBug(0);
            else             StartMatch(0);
            return;
        }
    }

    char solved[48];
    snprintf(solved, sizeof(solved), "Solved this session: %d", solvedCount);
    UiDrawCenteredText(solved, SCREEN_W / 2, 560, 22, COL_CYAN);

    if (BackPressed()) AppGoTo(app, SCREEN_MENU);
}

// ---------------------------------------------------------------------------
// Code Order
// ---------------------------------------------------------------------------
static void RunOrder(void)
{
    const OrderPuzzle *p = OrderPuzzleGet(orderIndex);
    DrawHeader("Code Order", p->title, p->goal);
    UiDrawCenteredText("Click a line, then click another line to swap them.", SCREEN_W / 2, 128, 18, COL_CYAN);

    Vector2 mouse = GetMousePosition();
    int clicked = -1;

    for (int i = 0; i < p->lineCount; i++)
    {
        Rectangle r = { ROW_X, ROW_TOP + i * ROW_STEP, ROW_W, ROW_H };
        bool hovered = !orderSolved && CheckCollisionPointRec(mouse, r);

        Color fill = CODE_BG;
        Color border = Fade(COL_MUTED, 0.5f);

        if (orderChecked)
        {
            bool ok = OrderLineCorrect(p, orderPerm, i);
            border = ok ? COL_DONE : COL_ERROR;
            if (ok) fill = RIGHT_FILL;
        }
        if (i == picked)
        {
            fill = Fade(COL_ACCENT, 0.2f);
            border = COL_ACCENT;
        }
        else if (hovered)
        {
            border = COL_ACCENT;
        }

        DrawCodeRow(r, i + 1, p->lines[orderPerm[i]], fill, border);

        if (hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) clicked = i;
    }

    if (clicked >= 0)
    {
        if (picked < 0)             picked = clicked;
        else if (picked == clicked) picked = -1;
        else
        {
            int tmp = orderPerm[picked];
            orderPerm[picked] = orderPerm[clicked];
            orderPerm[clicked] = tmp;
            picked = -1;
            orderChecked = false;
        }
    }

    if (orderSolved)
    {
        UiDrawCenteredText("Solved! That is the correct order.", SCREEN_W / 2, 606, 22, COL_DONE);
    }
    else if (orderChecked)
    {
        UiDrawCenteredText("Green lines are in the right place. Keep swapping.", SCREEN_W / 2, 606, 22, COL_MUTED);
    }

    if (UiButton((Rectangle){ 260, 644, 240, 52 }, "Check", !orderSolved, 1.0f))
    {
        orderChecked = true;
        picked = -1;
        if (OrderPuzzleSolved(p, orderPerm))
        {
            orderSolved = true;
            solvedCount++;
            UiToast("Solved!");
        }
        else
        {
            UiToast("Not quite yet");
        }
    }
    if (UiButton((Rectangle){ 520, 644, 240, 52 }, "Reshuffle", true, 1.0f))
    {
        StartOrder(orderIndex);
        return;
    }
    if (UiButton((Rectangle){ 780, 644, 240, 52 }, "Next puzzle", true, 1.0f))
    {
        StartOrder(orderIndex + 1);
        return;
    }

    if (BackPressed()) mode = MODE_HUB;
}

// ---------------------------------------------------------------------------
// Bug Hunt
// ---------------------------------------------------------------------------
static void RunBug(void)
{
    const BugPuzzle *p = BugPuzzleGet(bugIndex);
    DrawHeader("Bug Hunt", p->title, p->prompt);

    bool answered = bugChosen >= 0;
    Vector2 mouse = GetMousePosition();
    int clicked = -1;

    for (int i = 0; i < p->lineCount; i++)
    {
        Rectangle r = { ROW_X, ROW_TOP + i * ROW_STEP, ROW_W, ROW_H };
        bool hovered = !answered && CheckCollisionPointRec(mouse, r);

        Color fill = CODE_BG;
        Color border = Fade(COL_MUTED, 0.5f);

        if (answered)
        {
            if (i == p->bugLine)      { fill = RIGHT_FILL; border = COL_DONE; }
            else if (i == bugChosen)  { fill = WRONG_FILL; border = COL_ERROR; }
        }
        else if (hovered)
        {
            border = COL_ACCENT;
        }

        DrawCodeRow(r, i + 1, p->lines[i], fill, border);

        if (hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) clicked = i;
    }

    if (clicked >= 0)
    {
        bugChosen = clicked;
        if (clicked == p->bugLine) solvedCount++;
    }

    Rectangle panel = { ROW_X, 520, ROW_W, 100 };
    DrawRectangleRounded(panel, 0.12f, 8, COL_PANEL);

    if (!answered)
    {
        UiDrawText("Click the line that contains the bug.", (int)panel.x + 24, (int)panel.y + 36, 22, COL_MUTED);
    }
    else
    {
        bool ok = (bugChosen == p->bugLine);
        UiDrawText(ok ? "Found it!" : "Not that one. The bug is the green line.", (int)panel.x + 24, (int)panel.y + 14, 22, ok ? COL_DONE : COL_ERROR);
        UiDrawWrappedText(p->explanation, (Rectangle){ panel.x + 24, panel.y + 46, panel.width - 48, 50 }, 18, RAYWHITE);
    }

    if (UiButton((Rectangle){ SCREEN_W / 2.0f - 120, 644, 240, 52 }, "Next puzzle", answered, 1.0f))
    {
        StartBug(bugIndex + 1);
        return;
    }

    if (BackPressed()) mode = MODE_HUB;
}

// ---------------------------------------------------------------------------
// Match Up
// ---------------------------------------------------------------------------
static void RunMatch(float dt)
{
    const PairSet *set = PairSetGet(setIndex);
    DrawHeader("Match Up", set->title, "Flip two cards to match each term with its meaning.");

    char stats[64];
    snprintf(stats, sizeof(stats), "Moves: %d     Pairs: %d / %d", board.moves, board.pairsFound, PAIR_COUNT);
    UiDrawCenteredText(stats, SCREEN_W / 2, 132, 20, COL_CYAN);

    // Give the player a moment to see a wrong pair before it flips back
    if (MatchPending(&board))
    {
        resolveTimer -= dt;
        if (resolveTimer <= 0.0f && MatchResolve(&board) && MatchWon(&board))
        {
            solvedCount++;
            UiToast("All pairs matched!");
        }
    }

    float gridW = MATCH_COLS * MATCH_CARD_W + (MATCH_COLS - 1) * MATCH_GAP;
    float startX = (SCREEN_W - gridW) / 2.0f;
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < MATCH_CARDS; i++)
    {
        Rectangle r = {
            startX + (i % MATCH_COLS) * (MATCH_CARD_W + MATCH_GAP),
            180.0f + (i / MATCH_COLS) * (MATCH_CARD_H + MATCH_GAP),
            MATCH_CARD_W, MATCH_CARD_H
        };
        bool faceUp = board.matched[i] || i == board.first || i == board.second;
        bool hovered = !faceUp && !MatchPending(&board) && CheckCollisionPointRec(mouse, r);
        int side = MatchSideOf(&board, i);

        Color fill = COL_LOCKED;
        Color border = hovered ? COL_ACCENT : Fade(COL_MUTED, 0.5f);
        if (board.matched[i])
        {
            fill = RIGHT_FILL;
            border = COL_DONE;
        }
        else if (faceUp)
        {
            fill = COL_PANEL;
            border = side == 0 ? COL_CYAN : COL_ACCENT;
        }

        DrawRectangleRounded(r, 0.15f, 8, fill);
        DrawRectangleRoundedLinesEx(r, 0.15f, 8, 3.0f, border);

        if (faceUp)
        {
            int pair = MatchPairOf(&board, i);
            const char *text = side == 0 ? set->terms[pair] : set->meanings[pair];
            float textY = UiMeasureText(text, 20) <= (int)(r.width - 32) ? r.y + 40 : r.y + 24;
            UiDrawWrappedText(text, (Rectangle){ r.x + 16, textY, r.width - 32, 60 }, 20, RAYWHITE);
        }
        else
        {
            UiDrawCenteredText("?", (int)(r.x + r.width / 2), (int)r.y + 30, 40, Fade(COL_MUTED, 0.8f));
        }

        if (hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && MatchFlip(&board, i))
        {
            if (MatchPending(&board)) resolveTimer = RESOLVE_TIME;
        }
    }

    if (MatchWon(&board))
    {
        char win[64];
        snprintf(win, sizeof(win), "All matched in %d moves!", board.moves);
        UiDrawCenteredText(win, SCREEN_W / 2, 590, 26, COL_DONE);
    }

    if (UiButton((Rectangle){ SCREEN_W / 2.0f - 260, 644, 240, 52 }, "Restart", true, 1.0f))
    {
        StartMatch(setIndex);
        return;
    }
    if (UiButton((Rectangle){ SCREEN_W / 2.0f + 20, 644, 240, 52 }, "Next set", true, 1.0f))
    {
        StartMatch(setIndex + 1);
        return;
    }

    if (BackPressed()) mode = MODE_HUB;
}

// ---------------------------------------------------------------------------
void ScreenPuzzlesRun(AppState *app, float dt)
{
    switch (mode)
    {
    case MODE_HUB:   RunHub(app);   break;
    case MODE_ORDER: RunOrder();    break;
    case MODE_BUG:   RunBug();      break;
    case MODE_MATCH: RunMatch(dt);  break;
    }
}