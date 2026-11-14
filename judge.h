// Compiles the student's code with the computer's g++ and checks it against the
// problem's test cases. Raylib-free so it can be unit tested.
#ifndef JUDGE_H
#define JUDGE_H

#include <stdbool.h>
#include "problems.h"

typedef enum {
    JUDGE_OK,             // compiled and ran: look at the verdict for each test
    JUDGE_NO_COMPILER,    // g++ was not found
    JUDGE_COMPILE_ERROR,  // see compilerOutput
    JUDGE_INTERNAL        // could not write or run the temporary files
} JudgeStatus;

typedef enum {
    TEST_PASS,
    TEST_FAIL,       // ran but printed the wrong answer
    TEST_TIMEOUT,    // ran for more than 2 seconds
    TEST_CRASH,      // ended with an error
    TEST_SKIPPED     // not run because an earlier test timed out
} TestVerdict;

typedef struct {
    JudgeStatus status;
    char compilerOutput[1024];
    int testCount;
    int passed;
    TestVerdict verdict[MAX_TESTS];
    char actual[MAX_TESTS][128];   // what the program printed (shortened)
} JudgeResult;

bool JudgeCompilerAvailable(void);

// Takes about a second: the call blocks while compiling and running.
void JudgeRun(const Problem *problem, const char *studentCode, JudgeResult *result);

#endif