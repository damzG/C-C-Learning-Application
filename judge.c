#include "judge.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define NULL_DEVICE "NUL"
    #define RUN_PREFIX  ""
#else
    #include <sys/wait.h>
    #define NULL_DEVICE "/dev/null"
    #define RUN_PREFIX  "./"
#endif

#define SRC_FILE     "c2_run.cpp"
#define EXE_FILE     "c2_run"
#define INPUT_FILE   "c2_in.txt"
#define OUTPUT_FILE  "c2_out.txt"
#define COMPILE_FILE "c2_compile.txt"
#define HARNESS_LINE 100000

// Everything the student's code may need, and a watchdog that ends the program after
// 2 seconds. "#line 1" makes compiler messages match the editor's line numbers.
static const char *PRELUDE =
    "#include <iostream>\n#include <vector>\n#include <string>\n#include <algorithm>\n"
    "#include <map>\n#include <list>\n#include <cmath>\n#include <cstdio>\n#include <cstdlib>\n"
    "#include <cstring>\n#include <thread>\n#include <chrono>\n"
    "using namespace std;\n"
    "static void c2_watchdog() { std::thread([] { std::this_thread::sleep_for(std::chrono::milliseconds(2000)); std::_Exit(124); }).detach(); }\n"
    "#line 1\n";

static size_t ReadWholeFile(const char *path, char *buffer, size_t capacity)
{
    buffer[0] = '\0';
    FILE *f = fopen(path, "rb");
    if (f == NULL) return 0;
    size_t n = fread(buffer, 1, capacity - 1, f);
    buffer[n] = '\0';
    fclose(f);
    return n;
}

static bool WriteWholeFile(const char *path, const char *text)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL) return false;
    size_t len = strlen(text);
    bool ok = fwrite(text, 1, len, f) == len;
    return fclose(f) == 0 && ok;
}

// Same text for comparison: Windows line endings removed, trailing spaces and
// trailing blank lines ignored.
static void Normalize(const char *in, char *out, size_t capacity)
{
    size_t n = 0;
    for (const char *p = in; *p && n + 1 < capacity; p++)
    {
        if (*p == '\r') continue;
        out[n++] = *p;
    }
    out[n] = '\0';

    // Trim spaces before each newline and at the very end
    size_t w = 0;
    for (size_t r = 0; r < n; r++)
    {
        if (out[r] == '\n')
        {
            while (w > 0 && out[w - 1] == ' ') w--;
        }
        out[w++] = out[r];
    }
    out[w] = '\0';
    while (w > 0 && (out[w - 1] == '\n' || out[w - 1] == ' ')) out[--w] = '\0';
}

static int ExitCode(int status)
{
#ifdef _WIN32
    return status;
#else
    if (status == -1) return -1;
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    return 128 + (WIFSIGNALED(status) ? WTERMSIG(status) : 0);
#endif
}

static void RemoveTempFiles(void)
{
    remove(SRC_FILE);
    remove(EXE_FILE);
    remove(EXE_FILE ".exe");
    remove(INPUT_FILE);
    remove(OUTPUT_FILE);
    remove(COMPILE_FILE);
}

bool JudgeCompilerAvailable(void)
{
    static int cached = -1;
    if (cached < 0) cached = (system("g++ --version > " NULL_DEVICE " 2>&1") == 0) ? 1 : 0;
    return cached == 1;
}

// Turns g++'s long output into short lines such as "Line 3: expected ';'".
static void FormatCompilerErrors(const char *raw, char *out, size_t capacity)
{
    out[0] = '\0';
    size_t used = 0;
    int shown = 0;
    const char *p = raw;

    while (*p && shown < 3)
    {
        const char *eol = strchr(p, '\n');
        size_t len = eol ? (size_t)(eol - p) : strlen(p);

        char line[512];
        if (len >= sizeof(line)) len = sizeof(line) - 1;
        memcpy(line, p, len);
        line[len] = '\0';

        const char *marker = strstr(line, ": error: ");
        const char *prefix = strstr(line, SRC_FILE ":");
        if (marker != NULL && prefix != NULL)
        {
            int lineNo = atoi(prefix + strlen(SRC_FILE ":"));
            char entry[400];
            if (lineNo >= HARNESS_LINE)
            {
                snprintf(entry, sizeof(entry), "The tests could not call your function. Check its name, return type and parameters.\n");
            }
            else
            {
                snprintf(entry, sizeof(entry), "Line %d: %s\n", lineNo, marker + strlen(": error: "));
            }
            size_t el = strlen(entry);
            if (used + el + 1 < capacity)
            {
                memcpy(out + used, entry, el + 1);
                used += el;
                shown++;
            }
        }

        if (eol == NULL) break;
        p = eol + 1;
    }

    if (shown == 0)
    {
        snprintf(out, capacity, "%.300s", raw[0] ? raw : "The code could not be compiled.");
    }
}

void JudgeRun(const Problem *problem, const char *studentCode, JudgeResult *result)
{
    memset(result, 0, sizeof(*result));
    result->testCount = problem->testCount;

    if (!JudgeCompilerAvailable())
    {
        result->status = JUDGE_NO_COMPILER;
        return;
    }

    size_t capacity = strlen(PRELUDE) + strlen(studentCode) + strlen(problem->harness) + 64;
    char *source = (char *)malloc(capacity);
    if (source == NULL)
    {
        result->status = JUDGE_INTERNAL;
        return;
    }
    snprintf(source, capacity, "%s%s\n#line %d\n%s\n", PRELUDE, studentCode, HARNESS_LINE, problem->harness);

    bool written = WriteWholeFile(SRC_FILE, source);
    free(source);
    if (!written)
    {
        result->status = JUDGE_INTERNAL;
        snprintf(result->compilerOutput, sizeof(result->compilerOutput), "Could not write the temporary files here.");
        return;
    }

    int rc = system("g++ -std=c++17 -O1 -w -fmax-errors=3 -pthread " SRC_FILE " -o " EXE_FILE " 2> " COMPILE_FILE);
    if (rc != 0)
    {
        char raw[4096];
        ReadWholeFile(COMPILE_FILE, raw, sizeof(raw));
        FormatCompilerErrors(raw, result->compilerOutput, sizeof(result->compilerOutput));
        result->status = JUDGE_COMPILE_ERROR;
        RemoveTempFiles();
        return;
    }

    result->status = JUDGE_OK;
    bool skipRest = false;

    for (int t = 0; t < problem->testCount; t++)
    {
        if (skipRest)
        {
            result->verdict[t] = TEST_SKIPPED;
            continue;
        }

        if (!WriteWholeFile(INPUT_FILE, problem->tests[t].input))
        {
            result->status = JUDGE_INTERNAL;
            break;
        }

        int status = system(RUN_PREFIX EXE_FILE " < " INPUT_FILE " > " OUTPUT_FILE " 2> " NULL_DEVICE);
        int code = ExitCode(status);

        char rawOut[4096], got[4096], want[4096];
        ReadWholeFile(OUTPUT_FILE, rawOut, sizeof(rawOut));
        Normalize(rawOut, got, sizeof(got));
        Normalize(problem->tests[t].expected, want, sizeof(want));
        snprintf(result->actual[t], sizeof(result->actual[t]), "%.127s", got);

        if (code == 124)
        {
            result->verdict[t] = TEST_TIMEOUT;
            skipRest = true;   // a slow solution would be slow on every test
        }
        else if (code != 0)
        {
            result->verdict[t] = TEST_CRASH;
        }
        else if (strcmp(got, want) == 0)
        {
            result->verdict[t] = TEST_PASS;
            result->passed++;
        }
        else
        {
            result->verdict[t] = TEST_FAIL;
        }
    }

    RemoveTempFiles();
}