// Generated profile pictures (GitHub style identicons): raylib-free so it can be unit tested.
//
// A picture is a 5x5 grid of blocks, mirrored left to right, in one bright colour.
// The same name always gives the same picture, so nothing needs to be stored in the CSV.
#ifndef AVATAR_H
#define AVATAR_H

#include <stdbool.h>

#define AVATAR_GRID 5

typedef struct {
    bool cells[AVATAR_GRID][AVATAR_GRID];   // cells[row][column]
    unsigned char r, g, b;                  // colour of the filled blocks
} AvatarPattern;

// Case-insensitive, so "Ada_L" and "ada_l" get the same picture.
unsigned int AvatarSeedFromName(const char *name);

void AvatarGenerate(unsigned int seed, AvatarPattern *out);

#endif