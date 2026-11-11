// Unit tests for the avatar generator. Needs avatar.c only (no raylib).

#include <stdio.h>
#include <string.h>

#include "avatar.h"

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

static int CountFilled(const AvatarPattern *p)
{
    int n = 0;
    for (int r = 0; r < AVATAR_GRID; r++)
        for (int c = 0; c < AVATAR_GRID; c++)
            if (p->cells[r][c]) n++;
    return n;
}

static unsigned long Signature(const AvatarPattern *p)
{
    unsigned long sig = 0;
    for (int r = 0; r < AVATAR_GRID; r++)
        for (int c = 0; c < AVATAR_GRID; c++)
            sig = sig * 2 + (p->cells[r][c] ? 1 : 0);
    return sig * 16777216UL + ((unsigned long)p->r << 16) + ((unsigned long)p->g << 8) + p->b;
}

int test(void)
{
    // Same name, same picture
    AvatarPattern a, b;
    AvatarGenerate(AvatarSeedFromName("ada_l"), &a);
    AvatarGenerate(AvatarSeedFromName("ada_l"), &b);
    CHECK(memcmp(&a, &b, sizeof(a)) == 0);

    // Case does not matter
    AvatarGenerate(AvatarSeedFromName("ADA_L"), &b);
    CHECK(memcmp(&a, &b, sizeof(a)) == 0);

    // Every picture is mirrored, neither empty nor full, and brightly coloured
    char name[32];
    unsigned long seen[200];
    int distinct = 0;

    for (int i = 0; i < 200; i++)
    {
        snprintf(name, sizeof(name), "student%d", i);
        AvatarPattern p;
        AvatarGenerate(AvatarSeedFromName(name), &p);

        for (int r = 0; r < AVATAR_GRID; r++)
            for (int c = 0; c < AVATAR_GRID; c++)
                CHECK(p.cells[r][c] == p.cells[r][AVATAR_GRID - 1 - c]);

        int filled = CountFilled(&p);
        CHECK(filled >= 5 && filled <= 21);

        unsigned char maxChannel = p.r > p.g ? (p.r > p.b ? p.r : p.b) : (p.g > p.b ? p.g : p.b);
        CHECK(maxChannel >= 200);   // never a dull or dark colour

        unsigned long sig = Signature(&p);
        bool found = false;
        for (int k = 0; k < distinct; k++) if (seen[k] == sig) { found = true; break; }
        if (!found) seen[distinct++] = sig;
    }

    // Different names almost always look different
    CHECK(distinct >= 195);

    // Empty and odd names do not crash
    AvatarGenerate(AvatarSeedFromName(""), &a);
    CHECK(CountFilled(&a) > 0);
    AvatarGenerate(AvatarSeedFromName("?"), &a);

    if (failures == 0) printf("All avatar tests passed. (%d distinct of 200)\n", distinct);
    return failures == 0 ? 0 : 1;
}