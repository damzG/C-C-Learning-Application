#include "practice_progress.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define PRACTICE_MAX 2048
#define NAME_LEN     64
#define ID_LEN       48

typedef struct {
    char username[NAME_LEN];
    char problemId[ID_LEN];
} Entry;

static Entry entries[PRACTICE_MAX];
static int entryCount = 0;

static bool SameText(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return false;
        a++;
        b++;
    }
    return *a == *b;
}

void PracticeLoad(const char *path)
{
    entryCount = 0;

    FILE *f = fopen(path, "r");
    if (f == NULL) return;

    char line[256];
    while (fgets(line, sizeof(line), f) != NULL && entryCount < PRACTICE_MAX)
    {
        line[strcspn(line, "\r\n")] = '\0';
        char *comma = strchr(line, ',');
        if (comma == NULL || comma == line || comma[1] == '\0') continue;

        *comma = '\0';
        snprintf(entries[entryCount].username, NAME_LEN, "%.63s", line);
        snprintf(entries[entryCount].problemId, ID_LEN, "%.47s", comma + 1);
        entryCount++;
    }
    fclose(f);
}

bool PracticeIsSolved(const char *username, const char *problemId)
{
    for (int i = 0; i < entryCount; i++)
    {
        if (SameText(entries[i].username, username) && strcmp(entries[i].problemId, problemId) == 0) return true;
    }
    return false;
}

int PracticeSolvedCount(const char *username)
{
    int n = 0;
    for (int i = 0; i < entryCount; i++)
    {
        if (SameText(entries[i].username, username)) n++;
    }
    return n;
}

bool PracticeMarkSolved(const char *username, const char *problemId, const char *path)
{
    if (PracticeIsSolved(username, problemId) || entryCount >= PRACTICE_MAX) return false;

    snprintf(entries[entryCount].username, NAME_LEN, "%.63s", username);
    snprintf(entries[entryCount].problemId, ID_LEN, "%.47s", problemId);
    entryCount++;

    FILE *f = fopen(path, "a");
    if (f != NULL)
    {
        fprintf(f, "%s,%s\n", username, problemId);
        fclose(f);
    }
    return true;
}