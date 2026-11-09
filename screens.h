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

void ScreenPuzzlesEnter(AppState *app);
void ScreenPuzzlesRun(AppState *app, float dt);

#endif