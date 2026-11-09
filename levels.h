// Lesson map data: raylib-free.
#ifndef LEVELS_H
#define LEVELS_H

typedef struct {
    int section;
    const char *title;
    const char *summary;
    int xp;
} Level;

int LevelCount(void);
const Level *LevelGet(int index);

int SectionCount(void);
const char *SectionName(int section);

#endif