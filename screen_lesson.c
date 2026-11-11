// Lesson screen: quiz questions, then a short coding challenge, then a result.
#include "screens.h"
#include "theme.h"
#include "ui.h"
#include "levels.h"
#include "questions.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef enum { PHASE_ASK, PHASE_CODE, PHASE_RESULT } Phase;

// ---- BEGIN CHALLENGE LOGIC (no raylib calls) --------------------------------
// Coding challenges. A level can have several (up to MAX_LEVEL_CHALLENGES); they
// are matched to a level by the level field and played in the order listed. The
// student types the missing line of code; the answer is compared with a list of
// accepted answers, ignoring spaces. (The app cannot compile code, so this is a
// pattern check, which is why several correct spellings are listed.)
typedef struct {
    int level;                 // index into the level list
    const char *task;
    const char *before;        // code shown above the blank, lines split by \n, or NULL
    const char *after;         // code shown below the blank, or NULL
    const char *accepted[8];   // accepted[0] is shown as the answer; unused slots are NULL
    const char *hint;
} Challenge;

static const Challenge CHALLENGES[] = {
    // 0 Variables and Types
    { 0, "Declare an int variable called age and set it to 18.",
      "#include <iostream>\nusing namespace std;\n\nint main() {",
      "    cout << age;\n}",
      { "int age = 18;", "int age(18);", "int age{18};", "int age = {18};" },
      "Type, then name, then = and the value. End the line with a semicolon." },

    // 1 Loops and Functions
    { 1, "Write the for loop header that counts i from 1 up to and including 10.",
      "int sum = 0;",
      "    sum += i;\ncout << sum;",
      { "for (int i = 1; i <= 10; i++)", "for (int i = 1; i <= 10; ++i)",
        "for (int i = 1; i < 11; i++)", "for (int i = 1; i < 11; ++i)",
        "for (int i = 1; i <= 10; i += 1)" },
      "for (start; condition; step). The condition must still be true when i is 10." },

    // 1 Loops and Functions (Lab 1: leap year)
    { 1, "Write the return statement for isLeapYear. A leap year is divisible by 4, but not by 100 unless it is also divisible by 400.",
      "bool isLeapYear(int year)\n{",
      "}",
      { "return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;",
        "return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);",
        "return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);",
        "return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);" },
      "There are two ways to be a leap year: divisible by 400, or divisible by 4 but not by 100." },

    // 1 Loops and Functions (Lab 1: reverse a number)
    { 1, "Inside the loop, add the last digit to reversed. First shift the digits already in reversed one place to the left.",
      "int reversed = 0;\nwhile (testNumber != 0)\n{\n    int digit = testNumber % 10;",
      "    testNumber /= 10;\n}\nreturn reversed;",
      { "reversed = reversed * 10 + digit;", "reversed = 10 * reversed + digit;",
        "reversed = (10 * reversed) + digit;", "reversed = (reversed * 10) + digit;",
        "reversed = digit + reversed * 10;", "reversed = digit + 10 * reversed;" },
      "Multiply reversed by 10 to make room, then add digit. For 123 that builds 3, then 32, then 321." },

    // 2 Arrays
    { 2, "Store the value 99 in the last element of the five element array arr.",
      "int arr[5] = {1, 2, 3, 4, 5};",
      "cout << arr[4];",
      { "arr[4] = 99;", "arr[5 - 1] = 99;" },
      "Array indexes start at 0, so a 5 element array ends at index 4." },

    // 2 Arrays (Lab 1: find an element)
    { 2, "Complete the loop body so find returns the index where toFind is stored.",
      "int find(int size, int arr[], int toFind)\n{\n    for (int i = 0; i < size; i++)",
      "    return -1;\n}",
      { "if (arr[i] == toFind) return i;", "if (toFind == arr[i]) return i;",
        "if (arr[i] == toFind) { return i; }", "if (toFind == arr[i]) { return i; }" },
      "Compare arr[i] with toFind and return i as soon as they match. Only return -1 after the loop ends." },

    // 2 Arrays (Lab 1: reverse in place)
    { 2, "Complete the loop so it reverses the array in place by swapping the two ends and working towards the middle.",
      "void reverse(int size, int arr[])\n{\n    for (int i = 0; i < size / 2; i++)",
      "}",
      { "swap(arr[i], arr[size - 1 - i]);", "swap(arr[i], arr[size - i - 1]);",
        "std::swap(arr[i], arr[size - 1 - i]);", "std::swap(arr[i], arr[size - i - 1]);" },
      "The partner of index i is size - 1 - i. Swap those two elements." },

    // 3 Strings and Text
    { 3, "Complete the if so that it counts every space in the text. This is how you count words.",
      "char text[] = \"one two three\";\nint spaces = 0;\nfor (int i = 0; text[i] != '\\0'; i++)",
      "    spaces++;",
      { "if (text[i] == ' ')", "if (' ' == text[i])", "if (text[i] == 32)", "if (isspace(text[i]))" },
      "Compare the current character, text[i], with the space character ' ' using ==." },

    // 3 Strings and Text (Lab 1: what counts as a letter)
    { 3, "Write the return statement for isLetter. A letter is in the range A to Z or a to z.",
      "bool isLetter(char c)\n{",
      "}",
      { "return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');",
        "return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');",
        "return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z';",
        "return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';",
        "return isalpha(c);" },
      "Characters are numbers underneath, so you can compare them: c >= 'A' && c <= 'Z' covers the capitals." },

    // 4 Pointer Basics
    { 4, "Declare a pointer called p that points to the int x.",
      "int x = 5;",
      "cout << *p;",
      { "int *p = &x;", "int* p = &x;", "int *p(&x);", "int *p{&x};" },
      "Use * in the declaration and & to get the address of x." },

    // 5 Pointer Arithmetic
    { 5, "Print the third element of arr using the pointer p and arithmetic. Do not write arr[2].",
      "int arr[] = {10, 20, 30, 40};\nint *p = arr;",
      NULL,
      { "cout << *(p + 2);", "cout << *(arr + 2);",
        "cout << *(p + 2) << endl;", "cout << *(arr + 2) << endl;",
        "printf(\"%d\", *(p + 2));" },
      "p + 2 moves two elements along. Put * in front of the bracketed address to read the value." },

    // 6 Dynamic Memory
    { 6, "Free the array that was created with new. Choose the right form of delete.",
      "int *data = new int[5];\ndata[0] = 1;",
      "data = nullptr;",
      { "delete[] data;" },
      "Memory from new[] is freed with delete followed by square brackets." },

    // 7 Structs
    { 7, "Set the age field of the Student s to 20.",
      "struct Student {\n    string name;\n    int age;\n};\n\nStudent s;",
      "cout << s.age;",
      { "s.age = 20;" },
      "Use the dot operator to reach a field of a struct variable." },

    // 8 Classes and Objects
    { 8, "Create a Dog object called d, passing \"Rex\" to the constructor.",
      "class Dog {\npublic:\n    string name;\n    Dog(string n) { name = n; }\n};\n\nint main() {",
      "    cout << d.name;\n}",
      { "Dog d(\"Rex\");", "Dog d = Dog(\"Rex\");", "Dog d{\"Rex\"};", "Dog d = Dog{\"Rex\"};" },
      "Write the class name, the object name, then the constructor argument in brackets." },

    // 9 Linked Lists
    { 9, "Make the new node n point at the old head of the list.",
      "Node *n = new Node;\nn->value = 5;",
      "head = n;",
      { "n->next = head;", "(*n).next = head;" },
      "Link the new node to the old first node before you move head." },

    // 10 Templates
    { 10, "Write the first line of a function template that uses a type parameter called T.",
      NULL,
      "T biggest(T a, T b) {\n    return a > b ? a : b;\n}",
      { "template <typename T>", "template <class T>" },
      "It starts with the word template, then T is declared inside angle brackets." },

    // 11 STL Vectors
    { 11, "Add the number 7 to the end of the vector v.",
      "vector<int> v;",
      "cout << v.size();",
      { "v.push_back(7);", "v.emplace_back(7);" },
      "A vector member function adds a value at the back." },

    // 12 STL Lists
    { 12, "Write the loop header that walks through the list L using an iterator called it.",
      "list<int> L = {1, 2, 3};",
      "    cout << *it << \" \";",
      { "for (auto it = L.begin(); it != L.end(); it++)",
        "for (auto it = L.begin(); it != L.end(); ++it)",
        "for (list<int>::iterator it = L.begin(); it != L.end(); it++)",
        "for (list<int>::iterator it = L.begin(); it != L.end(); ++it)" },
      "Start at L.begin(), keep going while it != L.end(), and move on with it++." },

    // 13 STL Maps
    { 13, "Store the value 95 in the map scores under the key \"Ada\".",
      "map<string, int> scores;",
      "cout << scores[\"Ada\"];",
      { "scores[\"Ada\"] = 95;", "scores.insert({\"Ada\", 95});",
        "scores.insert(make_pair(\"Ada\", 95));", "scores.emplace(\"Ada\", 95);",
        "scores.insert(pair<string, int>(\"Ada\", 95));" },
      "Use square brackets with the key, or the insert function." },

    // 14 Boss Challenge
    { 14, "Add a new Dog, created on the heap with the name \"Rex\", to the vector pets.",
      "class Dog {\npublic:\n    string name;\n    Dog(string n) { name = n; }\n};\n\nvector<Dog*> pets;",
      NULL,
      { "pets.push_back(new Dog(\"Rex\"));", "pets.emplace_back(new Dog(\"Rex\"));" },
      "new Dog(\"Rex\") gives you a Dog*, which is exactly what the vector holds." }
};

#define CHALLENGE_COUNT ((int)(sizeof(CHALLENGES) / sizeof(CHALLENGES[0])))
#define MAX_LEVEL_CHALLENGES 3

typedef enum { CHECK_WRONG, CHECK_RIGHT, CHECK_NO_SEMICOLON } CheckResult;

static int ChallengeCount(int levelIndex)
{
    int n = 0;
    for (int i = 0; i < CHALLENGE_COUNT; i++)
    {
        if (CHALLENGES[i].level == levelIndex) n++;
    }
    return n;
}

// The index-th challenge of a level, or NULL
static const Challenge *ChallengeGet(int levelIndex, int index)
{
    int n = 0;
    for (int i = 0; i < CHALLENGE_COUNT; i++)
    {
        if (CHALLENGES[i].level != levelIndex) continue;
        if (n == index) return &CHALLENGES[i];
        n++;
    }
    return NULL;
}

static void StripSpaces(const char *src, char *dst, size_t size)
{
    size_t n = 0;
    for (; *src && n + 1 < size; src++)
    {
        if (!isspace((unsigned char)*src)) dst[n++] = *src;
    }
    dst[n] = '\0';
}

static CheckResult CheckChallenge(const Challenge *c, const char *typed)
{
    char got[160], want[160];
    StripSpaces(typed, got, sizeof(got));

    for (int i = 0; i < 8 && c->accepted[i] != NULL; i++)
    {
        StripSpaces(c->accepted[i], want, sizeof(want));
        if (strcmp(got, want) == 0) return CHECK_RIGHT;
    }

    // Right answer, but the final semicolon is missing?
    for (int i = 0; i < 8 && c->accepted[i] != NULL; i++)
    {
        StripSpaces(c->accepted[i], want, sizeof(want));
        size_t n = strlen(want);
        if (n > 1 && want[n - 1] == ';')
        {
            want[n - 1] = '\0';
            if (strcmp(got, want) == 0) return CHECK_NO_SEMICOLON;
        }
    }
    return CHECK_WRONG;
}
// ---- END CHALLENGE LOGIC -----------------------------------------------------

#define CODE_LINE_H 28.0f
#define CODE_BOX_H  48.0f

static Phase phase;
static int level;
static int total;
static int qIndex;
static int score;
static int chosen;
static bool answered;
static bool hintShown;
static bool right[LESSON_MAX_QUESTIONS];
static bool quizPassed;
static bool codePassed;
static bool passed;
static int xpEarned;

// Coding challenge state
static int numChallenges;                       // challenges in this level
static int codeIndex;                           // the one being solved now
static bool codeResult[MAX_LEVEL_CHALLENGES];   // solved without showing the answer
static UiTextBox codeBox;
static int attempts;
static bool codeSolved;
static bool codeRevealed;
static bool codeHintShown;
static char codeNote[200];
static Color codeNoteColor;

static void SetNote(Color color, const char *text)
{
    snprintf(codeNote, sizeof(codeNote), "%s", text);
    codeNoteColor = color;
}

// Clears the text box and feedback for the challenge about to be played
static void StartChallenge(void)
{
    UiTextBoxInit(&codeBox, (Rectangle){ 0, 0, 0, 0 }, "", "type the missing line here");
    attempts = 0;
    codeSolved = false;
    codeRevealed = false;
    codeHintShown = false;
    codeNote[0] = '\0';
    codeNoteColor = COL_MUTED;
}

static void ResetLesson(void)
{
    phase = PHASE_ASK;
    total = QuestionCount(level);
    if (total > LESSON_MAX_QUESTIONS) total = LESSON_MAX_QUESTIONS;   // right[] only holds this many
    qIndex = 0;
    score = 0;
    chosen = -1;
    answered = false;
    hintShown = false;
    quizPassed = false;
    codePassed = false;
    passed = false;
    xpEarned = 0;
    for (int i = 0; i < LESSON_MAX_QUESTIONS; i++) right[i] = false;

    numChallenges = ChallengeCount(level);
    if (numChallenges > MAX_LEVEL_CHALLENGES) numChallenges = MAX_LEVEL_CHALLENGES;
    codeIndex = 0;
    for (int i = 0; i < MAX_LEVEL_CHALLENGES; i++) codeResult[i] = false;
    StartChallenge();
}

void ScreenLessonEnter(AppState *app)
{
    level = app->activeLevel;
    ResetLesson();
}

static void FinishLesson(AppState *app)
{
    phase = PHASE_RESULT;
    codePassed = true;
    for (int i = 0; i < numChallenges; i++)
    {
        if (!codeResult[i]) codePassed = false;
    }
    passed = quizPassed && codePassed;

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
    if (qIndex >= total)
    {
        quizPassed = LessonPassed(score, total);
        if (numChallenges > 0) phase = PHASE_CODE;
        else                   FinishLesson(app);
    }
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

    if (phase != PHASE_RESULT)
    {
        int steps = total + numChallenges;
        char prog[40];
        float filled;

        if (phase == PHASE_ASK)
        {
            snprintf(prog, sizeof(prog), "Question %d of %d", qIndex + 1, total);
            filled = (float)qIndex + (answered ? 1.0f : 0.0f);
        }
        else
        {
            if (numChallenges > 1) snprintf(prog, sizeof(prog), "Coding challenge %d of %d", codeIndex + 1, numChallenges);
            else                   snprintf(prog, sizeof(prog), "Coding challenge");
            filled = (float)(total + codeIndex) + ((codeSolved || codeRevealed) ? 1.0f : 0.0f);
        }
        DrawText(prog, 940, 14, 18, COL_MUTED);

        Rectangle bar = { 940, 42, 300, 12 };
        DrawRectangleRounded(bar, 0.5f, 8, COL_BG);
        if (filled > 0) DrawRectangleRounded((Rectangle){ bar.x, bar.y, bar.width * filled / steps, bar.height }, 0.5f, 8, COL_DONE);
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

        const char *nextLabel = (qIndex + 1 < total) ? "Next" : (numChallenges > 0 ? "Challenge" : "See result");
        if (UiButton((Rectangle){ 1020, 653, 170, 50 }, nextLabel, true, 1.0f) || IsKeyPressed(KEY_ENTER))
        {
            NextQuestion(app);
        }
    }
}

// ---------------------------------------------------------------------------
// Coding challenge
// ---------------------------------------------------------------------------
static float DrawCodeLines(const char *code, float x, float y)
{
    if (code == NULL) return y;

    char line[128];
    int len = 0;
    for (const char *p = code; ; p++)
    {
        if (*p == '\n' || *p == '\0')
        {
            line[len] = '\0';
            DrawText(line, (int)x, (int)y + 3, 20, (Color){ 200, 230, 200, 255 });
            y += CODE_LINE_H;
            len = 0;
            if (*p == '\0') break;
        }
        else if (len < (int)sizeof(line) - 1)
        {
            line[len++] = *p;
        }
    }
    return y;
}

static void CheckCode(const Challenge *c)
{
    if (codeBox.len == 0)
    {
        SetNote(COL_MUTED, "Type your answer first.");
        return;
    }

    CheckResult result = CheckChallenge(c, codeBox.text);
    if (result == CHECK_RIGHT)
    {
        codeSolved = true;
        SetNote(COL_DONE, "Correct! Nice work.");
        return;
    }

    attempts++;
    if (result == CHECK_NO_SEMICOLON)
    {
        SetNote(COL_ACCENT, "So close! Don't forget the semicolon at the end of the line.");
    }
    else if (attempts >= 2)
    {
        SetNote(COL_ERROR, "Not quite. Use Show hint, then Answer if you are stuck.");
    }
    else
    {
        SetNote(COL_ERROR, "Not quite. Check the spelling, brackets and semicolon.");
    }
}

static void RevealAnswer(const Challenge *c)
{
    codeRevealed = true;
    snprintf(codeBox.text, TEXTBOX_LEN, "%s", c->accepted[0]);
    codeBox.len = (int)strlen(codeBox.text);
    SetNote(COL_ACCENT, "Answer shown. A revealed answer does not count towards passing.");
}

static void RunCode(AppState *app)
{
    const Challenge *c = ChallengeGet(level, codeIndex);
    if (c == NULL)
    {
        FinishLesson(app);
        return;
    }
    bool finished = codeSolved || codeRevealed;
    bool hasNext = codeIndex + 1 < numChallenges;

    char tag[40] = "CODING CHALLENGE";
    if (numChallenges > 1) snprintf(tag, sizeof(tag), "CODING CHALLENGE %d OF %d", codeIndex + 1, numChallenges);
    int tagW = MeasureText(tag, 16) + 28;
    DrawRectangleRounded((Rectangle){ 80, 92, (float)tagW, 28 }, 0.5f, 8, Fade(COL_CYAN, 0.25f));
    DrawText(tag, 94, 98, 16, COL_CYAN);

    float y = 132;
    y += UiDrawWrappedText(c->task, (Rectangle){ 80, y, 1120, 80 }, 24, RAYWHITE) + 10;

    // Code panel: the lines before, the box where the student types, the lines after
    int nBefore = UiCodeLineCount(c->before);
    int nAfter = UiCodeLineCount(c->after);
    Rectangle panel = { 80, y, 1120, (nBefore + nAfter) * CODE_LINE_H + CODE_BOX_H + 24.0f };
    DrawRectangleRounded(panel, 0.04f, 8, (Color){ 10, 12, 24, 255 });
    DrawRectangleRoundedLinesEx(panel, 0.04f, 8, 1.0f, Fade(COL_MUTED, 0.4f));

    float cy = panel.y + 12.0f;
    cy = DrawCodeLines(c->before, panel.x + 18, cy);

    codeBox.rect = (Rectangle){ panel.x + 40, cy + 4, panel.width - 80, CODE_BOX_H - 8 };
    cy += CODE_BOX_H;
    DrawCodeLines(c->after, panel.x + 18, cy);

    if (!finished) UiTextBoxUpdate(&codeBox);
    UiTextBoxDraw(&codeBox, !finished);

    // Bottom panel: hint or feedback on the left, buttons on the right
    Rectangle bottom = { 80, 646, 1120, 64 };
    DrawRectangleRounded(bottom, 0.2f, 8, COL_PANEL);

    if (codeNote[0]) UiDrawWrappedText(codeNote, (Rectangle){ 100, 656, 720, 40 }, 18, codeNoteColor);
    else             DrawText("Type the missing line, then press Enter or Check.", 100, 668, 20, COL_MUTED);

    // One button is the hint first, then (after two wrong tries) the answer
    const char *helpLabel = codeHintShown ? "Answer" : "Show hint";
    bool helpEnabled = !finished && (!codeHintShown || attempts >= 2);
    if (UiButton((Rectangle){ 850, 653, 150, 50 }, helpLabel, helpEnabled, 1.0f))
    {
        if (!codeHintShown)
        {
            char hint[200];
            codeHintShown = true;
            snprintf(hint, sizeof(hint), "Hint: %s", c->hint);
            SetNote(COL_CYAN, hint);
        }
        else
        {
            RevealAnswer(c);
        }
        return;
    }

    bool submit = UiButton((Rectangle){ 1020, 653, 170, 50 }, finished ? (hasNext ? "Next" : "See result") : "Check", true, 1.0f)
               || IsKeyPressed(KEY_ENTER);
    if (submit)
    {
        if (!finished)
        {
            CheckCode(c);
        }
        else
        {
            codeResult[codeIndex] = codeSolved;   // a revealed answer does not count
            if (hasNext)
            {
                codeIndex++;
                StartChallenge();
            }
            else
            {
                FinishLesson(app);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Result
// ---------------------------------------------------------------------------
static void RunResult(AppState *app)
{
    char line[128];
    bool hasChallenge = numChallenges > 0;

    UiDrawCenteredText(passed ? "Lesson complete!" : "Almost there!", SCREEN_W / 2, 130, 52, passed ? COL_DONE : COL_ACCENT);

    snprintf(line, sizeof(line), "%d / %d quiz answers correct", score, total);
    UiDrawCenteredText(line, SCREEN_W / 2, 210, 36, RAYWHITE);

    // One circle per question, plus a square for the coding challenge
    int slots = total + numChallenges;
    float startX = SCREEN_W / 2.0f - (slots - 1) * 22.0f;
    for (int i = 0; i < total; i++)
    {
        DrawCircleV((Vector2){ startX + i * 44.0f, 290 }, 14, right[i] ? COL_DONE : COL_ERROR);
    }
    int codeSolvedCount = 0;
    for (int i = 0; i < numChallenges; i++)
    {
        float cx = startX + (total + i) * 44.0f;
        DrawRectangleRounded((Rectangle){ cx - 14, 276, 28, 28 }, 0.3f, 6, codeResult[i] ? COL_DONE : COL_ERROR);
        if (codeResult[i]) codeSolvedCount++;
    }

    if (hasChallenge)
    {
        if (numChallenges == 1)
        {
            UiDrawCenteredText(codePassed ? "Coding challenge: passed" : "Coding challenge: answer was shown",
                               SCREEN_W / 2, 332, 22, codePassed ? COL_DONE : COL_ERROR);
        }
        else
        {
            snprintf(line, sizeof(line), "Coding challenges: %d of %d solved", codeSolvedCount, numChallenges);
            UiDrawCenteredText(line, SCREEN_W / 2, 332, 22, codePassed ? COL_DONE : COL_ERROR);
        }
    }

    if (passed && xpEarned > 0)
    {
        snprintf(line, sizeof(line), "+%d XP earned. The next level is unlocked!", xpEarned);
        UiDrawCenteredText(line, SCREEN_W / 2, 374, 24, COL_CYAN);
    }
    else if (passed)
    {
        UiDrawCenteredText("Great practice. XP is only awarded the first time.", SCREEN_W / 2, 374, 22, COL_MUTED);
    }
    else if (!quizPassed)
    {
        snprintf(line, sizeof(line), "You need at least %d quiz answers correct to pass. Have another go!", LessonPassMark(total));
        UiDrawCenteredText(line, SCREEN_W / 2, 374, 22, COL_MUTED);
    }
    else
    {
        UiDrawCenteredText("Solve every coding challenge without showing the answer to pass.", SCREEN_W / 2, 374, 22, COL_MUTED);
    }

    Account *acc = AppCurrentAccount(app);
    if (acc && acc->levelsCompleted >= LevelCount())
    {
        UiDrawCenteredText("You have completed every level on the map!", SCREEN_W / 2, 414, 22, COL_ACCENT);
    }

    if (UiButton((Rectangle){ SCREEN_W / 2.0f - 270, 470, 250, 56 }, "Back to Map", true, 1.0f))
    {
        AppGoTo(app, SCREEN_MAP);
    }
    if (UiButton((Rectangle){ SCREEN_W / 2.0f + 20, 470, 250, 56 }, passed ? "Practice Again" : "Try Again", true, 1.0f))
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

    if (phase == PHASE_ASK)       RunQuestion(app);
    else if (phase == PHASE_CODE) RunCode(app);
    else                          RunResult(app);

    DrawHud(app);
}