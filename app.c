#include "app.h"
#include "screens.h"
#include "ui.h"
#include "theme.h"
#include "focus_audio.h"

#include "raylib.h"

void AppInit(AppState *app)
{
    app->current = -1;
    app->activeLevel = 0;
    app->activeProblem = 0;
    app->quit = false;
    app->focusMode = false;
    AccessibilitySettingsLoad(&app->accessibility);
    ThemeSetColorBlind(app->accessibility.colorBlindPalette);
    UiSetAccessibilitySettings(&app->accessibility);
    FocusAudioSetVolume(app->accessibility.musicVolume);
    ThemeLoad();

    if (!AccountsLoad(&app->accounts, ACCOUNTS_FILE))
    {
        TraceLog(LOG_WARNING, "Could not read %s", ACCOUNTS_FILE);
    }
    TraceLog(LOG_INFO, "Loaded %d profile(s)", app->accounts.count);

    AppGoTo(app, SCREEN_LOADING);
}

void AppGoTo(AppState *app, ScreenId screen)
{
    app->screen = screen;

    switch (screen)
    {
    case SCREEN_LOADING:  ScreenLoadingEnter(app);  break;
    case SCREEN_MENU:     ScreenMenuEnter(app);     break;
    case SCREEN_PROFILES: ScreenProfilesEnter(app); break;
    case SCREEN_CREATE:   ScreenCreateEnter(app);   break;
    case SCREEN_MAP:      ScreenMapEnter(app);      break;
    case SCREEN_LESSON:   ScreenLessonEnter(app);   break;
    case SCREEN_PRACTICE: ScreenPracticeEnter(app); break;
    case SCREEN_CODE:     ScreenCodeEnter(app);     break;
    case SCREEN_PUZZLES:  ScreenPuzzlesEnter(app);  break;
    case SCREEN_EXPLORE:  ScreenExploreEnter(app);  break;
    case SCREEN_STUDY:    ScreenStudyEnter(app);    break;
    case SCREEN_PARK:     ScreenParkEnter(app);     break;
    case SCREEN_ANALYTICS: ScreenAnalyticsEnter(app); break;
    case SCREEN_SETTINGS: ScreenSettingsEnter(app); break;
    }
}

void AppRunScreen(AppState *app, float dt)
{
    switch (app->screen)
    {
    case SCREEN_LOADING:  ScreenLoadingRun(app, dt);  break;
    case SCREEN_MENU:     ScreenMenuRun(app, dt);     break;
    case SCREEN_PROFILES: ScreenProfilesRun(app, dt); break;
    case SCREEN_CREATE:   ScreenCreateRun(app, dt);   break;
    case SCREEN_MAP:      ScreenMapRun(app, dt);      break;
    case SCREEN_LESSON:   ScreenLessonRun(app, dt);   break;
    case SCREEN_PRACTICE: ScreenPracticeRun(app, dt); break;
    case SCREEN_CODE:     ScreenCodeRun(app, dt);     break;
    case SCREEN_PUZZLES:  ScreenPuzzlesRun(app, dt);  break;
    case SCREEN_EXPLORE:  ScreenExploreRun(app, dt);  break;
    case SCREEN_STUDY:    ScreenStudyRun(app, dt);    break;
    case SCREEN_PARK:     ScreenParkRun(app, dt);     break;
    case SCREEN_ANALYTICS: ScreenAnalyticsRun(app, dt); break;
    case SCREEN_SETTINGS: ScreenSettingsRun(app, dt); break;
    }
}

Account *AppCurrentAccount(AppState *app)
{
    if (app->current < 0 || app->current >= app->accounts.count) return 0;
    return &app->accounts.items[app->current];
}

void AppSaveAccounts(AppState *app)
{
    if (!AccountsSave(&app->accounts, ACCOUNTS_FILE))
    {
        TraceLog(LOG_ERROR, "Could not write %s", ACCOUNTS_FILE);
        UiToast("Could not save accounts.csv");
    }
}