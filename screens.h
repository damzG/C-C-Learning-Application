// One Enter function (reset state) and one Run function (update and draw) per screen.
#ifndef SCREENS_H
#define SCREENS_H

#include "app.h"

void ScreenLoadingEnter(AppState *app);
void ScreenLoadingRun(AppState *app, float dt);

void ScreenMenuEnter(AppState *app);
void ScreenMenuRun(AppState *app, float dt);

void ScreenProfilesEnter(AppState *app);
void ScreenProfilesRun(AppState *app, float dt);

void ScreenCreateEnter(AppState *app);
void ScreenCreateRun(AppState *app, float dt);

void ScreenMapEnter(AppState *app);
void ScreenMapRun(AppState *app, float dt);

void ScreenLessonEnter(AppState *app);
void ScreenLessonRun(AppState *app, float dt);

void ScreenPracticeEnter(AppState *app);
void ScreenPracticeRun(AppState *app, float dt);

void ScreenCodeEnter(AppState *app);
void ScreenCodeRun(AppState *app, float dt);

void ScreenPuzzlesEnter(AppState *app);
void ScreenPuzzlesRun(AppState *app, float dt);

void ScreenExploreEnter(AppState *app);
void ScreenExploreRun(AppState *app, float dt);

void ScreenStudyEnter(AppState *app);
void ScreenStudyRun(AppState *app, float dt);

void ScreenAnalyticsEnter(AppState *app);
void ScreenAnalyticsRun(AppState *app, float dt);
void ScreenSettingsEnter(AppState *app);
void ScreenSettingsRun(AppState *app, float dt);

void ScreenParkEnter(AppState *app);
void ScreenParkRun(AppState *app, float dt);
void ScreenRevisionEnter(AppState *app);
void ScreenRevisionRun(AppState *app, float dt);
void ScreenTutorialsEnter(AppState *app);
void ScreenTutorialsRun(AppState *app, float dt);

#endif
