// Application state and screen routing.
#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include "account.h"
#include "accessibility_settings.h"

typedef enum {
    SCREEN_LOADING,
    SCREEN_MENU,
    SCREEN_PROFILES,
    SCREEN_CREATE,
    SCREEN_MAP,
    SCREEN_LESSON,
    SCREEN_PRACTICE,
    SCREEN_CODE,
    SCREEN_PUZZLES,
    SCREEN_EXPLORE,
    SCREEN_STUDY,
    SCREEN_PARK,
    SCREEN_ANALYTICS,
    SCREEN_SETTINGS
} ScreenId;

typedef struct {
    ScreenId screen;
    AccountStore accounts;
    int current;       // index of the signed in account, or -1
    int activeLevel;   // level being played in the lesson screen
    int activeProblem; // problem open in the code practice screen
    bool quit;
    bool focusMode;
    AccessibilitySettings accessibility;
} AppState;

void AppInit(AppState *app);
void AppGoTo(AppState *app, ScreenId screen);
void AppRunScreen(AppState *app, float dt);

Account *AppCurrentAccount(AppState *app);
void AppSaveAccounts(AppState *app);

#endif