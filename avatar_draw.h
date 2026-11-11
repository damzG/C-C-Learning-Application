// Draws a generated profile picture with raylib.
#ifndef AVATAR_DRAW_H
#define AVATAR_DRAW_H

#include "raylib.h"

// (x, y) is the top left corner and size is the width and height in pixels.
void AvatarDraw(const char *name, int x, int y, int size, float alpha);

#endif