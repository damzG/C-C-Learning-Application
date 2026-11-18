#ifndef CHESS_GAME_H
#define CHESS_GAME_H

#include "app.h"
#include <stdbool.h>

void ChessGameEnter(void);
// Returns true when the player clicks Back or presses ESC
bool ChessGameRun(AppState *app, float dt);

#endif