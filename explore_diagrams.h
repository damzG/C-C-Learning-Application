#ifndef EXPLORE_DIAGRAMS_H
#define EXPLORE_DIAGRAMS_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    DIAG_CAT_CS = 0,
    DIAG_CAT_AI,
    DIAG_CAT_SYS_DESIGN,
    DIAG_CAT_COUNT
} DiagramCategory;

void ExploreDiagramsInit(void);
void ExploreDiagramsDraw(Rectangle bounds, DiagramCategory category, float dt);

#endif