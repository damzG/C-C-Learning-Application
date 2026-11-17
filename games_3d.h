// 3D knowledge games (Raylib camera + mesh drawing).
#ifndef GAMES_3D_H
#define GAMES_3D_H

#include "app.h"

typedef enum {
    GAME3D_BLOCK_DROP = 0,
    GAME3D_CODE_CARDS,
    GAME3D_COUNT
} Game3dId;

typedef struct {
    Game3dId id;
    const char *title;
    const char *blurb;
    const char *tag;
} Game3dInfo;

int Game3dCount(void);
const Game3dInfo *Game3dGet(int index);

void Game3dEnter(Game3dId id);
bool Game3dRun(AppState *app, float dt);

#endif
