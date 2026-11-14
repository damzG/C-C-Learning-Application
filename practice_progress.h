// Remembers which practice problems each player has solved: raylib-free.
#ifndef PRACTICE_PROGRESS_H
#define PRACTICE_PROGRESS_H

#include <stdbool.h>

#define PRACTICE_FILE "practice.csv"

void PracticeLoad(const char *path);
bool PracticeIsSolved(const char *username, const char *problemId);
int  PracticeSolvedCount(const char *username);

// Returns true the first time a problem is solved (and saves it), false for repeats.
bool PracticeMarkSolved(const char *username, const char *problemId, const char *path);

#endif