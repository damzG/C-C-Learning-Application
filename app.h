// Application state and screen routing.
#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include "account.h"

typedef enum {
    SCREEN_LOADING,
    SCREEN_MENU,
    SCREEN_PROFILES,
    SCREEN_CREATE,
    SCREEN_MAP,
    SCREEN_PUZZLES
} ScreenId;

typedef struct {
    ScreenId screen;
    AccountStore accounts;
    int current;   // index of the signed in account, or -1
    bool quit;
} AppState;

void AppInit(AppState *app);
void AppGoTo(AppState *app, ScreenId screen);
void AppRunScreen(AppState *app, float dt);

Account *AppCurrentAccount(AppState *app);
void AppSaveAccounts(AppState *app);

#endif