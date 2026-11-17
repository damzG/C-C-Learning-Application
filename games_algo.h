// Extra 2D algorithm / coding games used by the Puzzle Games hub.
#ifndef GAMES_ALGO_H
#define GAMES_ALGO_H

#include "app.h"

typedef enum {
    GAME_SORT_SPRINT = 0,
    GAME_BINARY_HUNT,
    GAME_TREE_ARCHITECT,
    GAME_PATH_FINDER,
    GAME_BIG_O,
    GAME_STACK_ATTACK,
    GAME_CODE_TRACER,
    GAME_REFACTOR_PUSH,
    GAME_ALGO_COUNT
} AlgoGameId;

typedef struct {
    AlgoGameId id;
    const char *title;
    const char *blurb;
    const char *tag;
} AlgoGameInfo;

int AlgoGameCount(void);
const AlgoGameInfo *AlgoGameGet(int index);

void AlgoGameEnter(AlgoGameId id);
// Returns true when the player pressed Back to leave the game.
bool AlgoGameRun(AppState *app, float dt);

#endif
