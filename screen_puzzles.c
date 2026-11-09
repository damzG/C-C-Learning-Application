#include "screens.h"
#include "theme.h"
#include "ui.h"

void ScreenPuzzlesEnter(AppState *app)
{
    (void)app;
}

void ScreenPuzzlesRun(AppState *app, float dt)
{
    (void)dt;

    UiDrawCenteredText("Puzzle Games", SCREEN_W / 2, 220, 48, RAYWHITE);
    UiDrawCenteredText("Code puzzles are coming soon.", SCREEN_W / 2, 300, 24, COL_MUTED);

    if (UiButton((Rectangle){ SCREEN_W / 2.0f - 120, 400, 240, 56 }, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE))
    {
        AppGoTo(app, SCREEN_MENU);
    }
}