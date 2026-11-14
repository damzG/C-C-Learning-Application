// Tests for the practice problems, the judge and the progress file.
// The judge tests need g++ on the PATH and are skipped if it is missing.

#include <stdio.h>
#include <string.h>

#include "judge.h"
#include "practice_progress.h"
#include "problems.h"

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

static const Problem *Find(const char *id)
{
    for (int i = 0; i < ProblemCount(); i++)
        if (strcmp(ProblemGet(i)->id, id) == 0) return ProblemGet(i);
    return NULL;
}

int main(void)
{
    // Problem data is complete and ids are unique
    for (int i = 0; i < ProblemCount(); i++)
    {
        const Problem *p = ProblemGet(i);
        CHECK(p->id && p->title && p->description && p->starter && p->solution && p->harness);
        CHECK(p->testCount >= 3 && p->testCount <= MAX_TESTS);
        for (int h = 0; h < HINT_COUNT; h++) CHECK(p->hints[h] != NULL);
        for (int t = 0; t < p->testCount; t++) CHECK(p->tests[t].input && p->tests[t].expected);
        for (int j = i + 1; j < ProblemCount(); j++) CHECK(strcmp(p->id, ProblemGet(j)->id) != 0);
    }
    CHECK(ProblemGet(-1) == NULL && ProblemGet(ProblemCount()) == NULL);

    // Progress file
    const char *path = "test_practice.csv";
    remove(path);
    PracticeLoad(path);
    CHECK(!PracticeIsSolved("ada_l", "leap-year"));
    CHECK(PracticeMarkSolved("ada_l", "leap-year", path));
    CHECK(!PracticeMarkSolved("ADA_L", "leap-year", path));
    CHECK(PracticeMarkSolved("ada_l", "prime-number", path));
    CHECK(PracticeMarkSolved("grace", "leap-year", path));
    PracticeLoad(path);
    CHECK(PracticeIsSolved("ada_l", "leap-year"));
    CHECK(PracticeSolvedCount("ada_l") == 2);
    CHECK(PracticeSolvedCount("grace") == 1);
    CHECK(!PracticeIsSolved("grace", "prime-number"));
    remove(path);

    if (!JudgeCompilerAvailable())
    {
        printf("g++ not found: judge tests skipped.\n");
        return failures == 0 ? 0 : 1;
    }

    // Every reference solution passes every test
    JudgeResult r;
    for (int i = 0; i < ProblemCount(); i++)
    {
        const Problem *p = ProblemGet(i);
        JudgeRun(p, p->solution, &r);
        if (r.status != JUDGE_OK || r.passed != p->testCount)
            printf("  problem '%s': status %d, %d of %d passed. %s\n", p->id, r.status, r.passed, p->testCount, r.compilerOutput);
        CHECK(r.status == JUDGE_OK);
        CHECK(r.passed == p->testCount);
    }

    // The starter code compiles but does not pass
    const Problem *leap = Find("leap-year");
    JudgeRun(leap, leap->starter, &r);
    CHECK(r.status == JUDGE_OK && r.passed < leap->testCount);

    // A plausible wrong answer fails on the century years
    JudgeRun(leap, "bool isLeapYear(int year) { return year % 4 == 0; }\n", &r);
    CHECK(r.status == JUDGE_OK);
    CHECK(r.verdict[1] == TEST_FAIL);   // 1900
    CHECK(r.verdict[0] == TEST_PASS);   // 2000

    // Compile errors report the editor's line number
    JudgeRun(leap, "bool isLeapYear(int year) {\n    return year % 4 == 0\n}\n", &r);
    CHECK(r.status == JUDGE_COMPILE_ERROR);
    CHECK(strstr(r.compilerOutput, "Line 2") != NULL || strstr(r.compilerOutput, "Line 3") != NULL);

    // Wrong function name: told that the tests could not call it
    JudgeRun(leap, "bool leap(int year) { return true; }\n", &r);
    CHECK(r.status == JUDGE_COMPILE_ERROR);
    CHECK(strstr(r.compilerOutput, "could not call your function") != NULL);

    // An infinite loop is stopped
    const Problem *sum = Find("sum-of-digits");
    JudgeRun(sum, "int sumDigits(int n) { while (true) {} return 0; }\n", &r);
    CHECK(r.status == JUDGE_OK);
    CHECK(r.verdict[0] == TEST_TIMEOUT);
    CHECK(r.verdict[1] == TEST_SKIPPED);

    // A crash is reported
    JudgeRun(sum, "int sumDigits(int n) { int *p = nullptr; return *p; }\n", &r);
    CHECK(r.status == JUDGE_OK && r.verdict[0] == TEST_CRASH);

    // Windows style line endings in the output do not matter (checked through the solution path above)
    if (failures == 0) printf("All practice tests passed.\n");
    return failures == 0 ? 0 : 1;
}