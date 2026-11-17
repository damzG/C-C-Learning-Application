// The Panda hint bot. Drawn with raylib shapes, so it needs no image files.
// If assets/panda.png exists it is used instead (one square picture for every mood).
#ifndef PANDA_H
#define PANDA_H

#include "raylib.h"

typedef enum {
    PANDA_IDLE,    // waiting, blinks now and then
    PANDA_HAPPY,   // right answer
    PANDA_SAD,     // wrong answer
    PANDA_THINK    // a hint is showing
} PandaMood;

// (x, y) is the top left corner of a square of the given size.
void PandaDraw(int x, int y, int size, PandaMood mood, float alpha);

// Frees the optional picture. Call once before CloseWindow.
void PandaShutdown(void);

#endif
