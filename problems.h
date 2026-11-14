// Code practice problems: raylib-free. Every problem carries its own hints, reference
// solution and test cases, so the app can mark answers without any server.
#ifndef PROBLEMS_H
#define PROBLEMS_H

#define MAX_TESTS   8
#define HINT_COUNT  3

typedef enum { DIFF_EASY, DIFF_MEDIUM } Difficulty;

typedef struct {
    const char *input;      // text sent to the test program's standard input
    const char *expected;   // text it must print
} TestCase;

typedef struct {
    const char *id;           // short stable name, used when saving progress
    const char *title;
    Difficulty difficulty;
    const char *topic;
    const char *source;       // for example "Lab Q1"
    const char *description;
    const char *starter;      // code shown in the editor at the start
    const char *hints[HINT_COUNT];
    const char *solution;     // reference answer, shown on request
    const char *harness;      // hidden code with main(): reads a test, calls the student's function
    int testCount;
    TestCase tests[MAX_TESTS];
    int xp;
} Problem;

int ProblemCount(void);
const Problem *ProblemGet(int index);      // NULL when out of range
const char *DifficultyName(Difficulty d);

#endif