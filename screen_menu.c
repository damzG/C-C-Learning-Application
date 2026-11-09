#include "screens.h"
#include "theme.h"
#include "ui.h"

#include <math.h>
#include <stdio.h>

static float fade;

void ScreenMenuEnter(AppState *app)
{
    (void)app;
    fade = 0.0f;
}

void ScreenMenuRun(AppState *app, float dt)
{
    fade = fminf(fade + dt * 2.0f, 1.0f);

    UiDrawTrack(0.5f * fade);
    UiDrawLogo(SCREEN_W / 2, 130, 100, fade);

    Account *acc = AppCurrentAccount(app);
    if (acc)
    {
        char line[160];
        snprintf(line, sizeof(line), "Signed in as %s  |  %d XP", acc->fullName, acc->xp);
        int w = MeasureText(line, 20);
        DrawText(line, SCREEN_W - w - 24, 20, 20, Fade(COL_MUTED, fade));
    }

    float bw = 380.0f, bh = 62.0f;
    float bx = (SCREEN_W - bw) / 2.0f;
    float by = 240.0f;
    float step = 80.0f;

    if (UiButton((Rectangle){ bx, by, bw, bh }, acc ? "Continue Learning" : "Start Learning", true, fade))
    {
        if (acc)                          AppGoTo(app, SCREEN_MAP);
        else if (app->accounts.count > 0) AppGoTo(app, SCREEN_PROFILES);
        else
        {
            UiToast("Create a profile to start learning");
            AppGoTo(app, SCREEN_CREATE);
        }
    }

    if (UiButton((Rectangle){ bx, by + step, bw, bh }, acc ? "Switch Profile" : "Profiles / Sign Up", true, fade))
    {
        AppGoTo(app, app->accounts.count > 0 ? SCREEN_PROFILES : SCREEN_CREATE);
    }

    if (UiButton((Rectangle){ bx, by + step * 2, bw, bh }, "Puzzle Games", true, fade)) AppGoTo(app, SCREEN_PUZZLES);
    if (UiButton((Rectangle){ bx, by + step * 3, bw, bh }, "Quit", true, fade)) app->quit = true;

    DrawText("v0.2  early development", 20, SCREEN_H - 30, 18, Fade(COL_MUTED, fade));
}