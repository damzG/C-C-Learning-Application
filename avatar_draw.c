#include "avatar_draw.h"
#include "avatar.h"

void AvatarDraw(const char *name, int x, int y, int size, float alpha)
{
    AvatarPattern pattern;
    AvatarGenerate(AvatarSeedFromName(name), &pattern);

    Color fg = { pattern.r, pattern.g, pattern.b, 255 };
    Color bg = { 236, 238, 246, 255 };

    DrawRectangleRounded((Rectangle){ (float)x, (float)y, (float)size, (float)size }, 0.12f, 6, Fade(bg, alpha));

    // Whole pixel cells keep the blocks crisp, with no hairline gaps between them
    int cell = (int)(size * 0.8f) / AVATAR_GRID;
    if (cell < 1) cell = 1;
    int inner = cell * AVATAR_GRID;
    int ox = x + (size - inner) / 2;
    int oy = y + (size - inner) / 2;

    for (int row = 0; row < AVATAR_GRID; row++)
    {
        for (int col = 0; col < AVATAR_GRID; col++)
        {
            if (pattern.cells[row][col])
            {
                DrawRectangle(ox + col * cell, oy + row * cell, cell, cell, Fade(fg, alpha));
            }
        }
    }
}