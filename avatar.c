#include "avatar.h"

#include <ctype.h>

#define MIN_FILLED 9    // keeps pictures from looking empty
#define MAX_FILLED 19   // or completely full

unsigned int AvatarSeedFromName(const char *name)
{
    // FNV-1a hash of the lower-case name
    unsigned int h = 2166136261u;
    for (const unsigned char *p = (const unsigned char *)name; *p; p++)
    {
        h ^= (unsigned int)tolower(*p);
        h *= 16777619u;
    }
    return h;
}

// Small deterministic random number generator. Names that differ by one letter
// still end up with very different pictures because the bits are well mixed.
static unsigned int Mix(unsigned int x)
{
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

static unsigned int Next(unsigned int *state)
{
    *state += 0x9e3779b9U;
    return Mix(*state);
}

static void HsvToRgb(float h, float s, float v, unsigned char *r, unsigned char *g, unsigned char *b)
{
    float sector = h / 60.0f;
    int i = (int)sector;
    float f = sector - (float)i;

    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);

    float rr, gg, bb;
    switch (i % 6)
    {
        case 0:  rr = v; gg = t; bb = p; break;
        case 1:  rr = q; gg = v; bb = p; break;
        case 2:  rr = p; gg = v; bb = t; break;
        case 3:  rr = p; gg = q; bb = v; break;
        case 4:  rr = t; gg = p; bb = v; break;
        default: rr = v; gg = p; bb = q; break;
    }

    *r = (unsigned char)(rr * 255.0f + 0.5f);
    *g = (unsigned char)(gg * 255.0f + 0.5f);
    *b = (unsigned char)(bb * 255.0f + 0.5f);
}

void AvatarGenerate(unsigned int seed, AvatarPattern *out)
{
    unsigned int state = seed;

    // Fill the left half and the middle column, then mirror. If the result is too
    // empty or too full, draw again from the same generator (still deterministic).
    for (int attempt = 0; attempt < 8; attempt++)
    {
        int filled = 0;

        for (int row = 0; row < AVATAR_GRID; row++)
        {
            unsigned int bits = Next(&state);
            for (int col = 0; col <= AVATAR_GRID / 2; col++)
            {
                bool on = ((bits >> col) & 1u) != 0;
                int mirror = AVATAR_GRID - 1 - col;

                out->cells[row][col] = on;
                out->cells[row][mirror] = on;

                if (on) filled += (col == mirror) ? 1 : 2;
            }
        }

        if (filled >= MIN_FILLED && filled <= MAX_FILLED) break;
    }

    unsigned int c = Next(&state);
    float hue = (float)(c % 360u);
    float sat = 0.55f + (float)((c >> 9) % 25u) / 100.0f;    // 0.55 to 0.79
    float val = 0.85f + (float)((c >> 16) % 10u) / 100.0f;   // 0.85 to 0.94
    HsvToRgb(hue, sat, val, &out->r, &out->g, &out->b);
}