#include "screens.h"
#include "theme.h"
#include "focus_audio.h"
#include "ui.h"
#include "avatar_draw.h"
#include "park_bg.h"
#include "theme.h"

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

    ParkBgDraw(0.45f * fade);
    UiDrawTrack(0.5f * fade);
    UiDrawLogo(SCREEN_W / 2, 130, 100, fade);

    Account *acc = AppCurrentAccount(app);
    if (acc)
    {
        char line[160];
        snprintf(line, sizeof(line), "Signed in as %s  |  %d XP", acc->fullName, acc->xp);
        int w = UiMeasureText(line, 20);
        int textX = SCREEN_W - w - 24;
        AvatarDraw(acc->username, textX - 52, 8, 40, fade);
        UiDrawText(line, textX, 20, 20, Fade(COL_MUTED, fade));
    }

    float bw = 360.0f, bh = 52.0f;
    float gap = 14.0f;
    float col0 = SCREEN_W / 2.0f - bw - gap / 2.0f;
    float col1 = SCREEN_W / 2.0f + gap / 2.0f;
    float by = 220.0f;
    float step = bh + 16.0f;

    if (UiButtonPrimary((Rectangle){ col0, by, bw * 2 + gap, bh }, acc ? "Continue Learning" : "Start Learning", true, fade))
    {
        if (acc)                          AppGoTo(app, SCREEN_MAP);
        else if (app->accounts.count > 0) AppGoTo(app, SCREEN_PROFILES);
        else
        {
            UiToast("Create a profile to start learning");
            AppGoTo(app, SCREEN_CREATE);
        }
    }

    if (UiButton((Rectangle){ col0, by + step, bw, bh }, acc ? "Switch Profile" : "Profiles / Sign Up", true, fade))
    {
        AppGoTo(app, app->accounts.count > 0 ? SCREEN_PROFILES : SCREEN_CREATE);
    }
    if (UiButton((Rectangle){ col1, by + step, bw, bh }, "Code Practice", true, fade)) AppGoTo(app, SCREEN_PRACTICE);

    if (UiButton((Rectangle){ col0, by + step * 2, bw, bh }, "Explore (DSA & CS)", true, fade)) AppGoTo(app, SCREEN_EXPLORE);
    if (UiButton((Rectangle){ col1, by + step * 2, bw, bh }, "Puzzle Games", true, fade)) AppGoTo(app, SCREEN_PUZZLES);

    if (UiButton((Rectangle){ col0, by + step * 3, bw, bh }, "Study Tools", true, fade)) AppGoTo(app, SCREEN_STUDY);
    if (UiButton((Rectangle){ col1, by + step * 3, bw, bh }, "C^2 Park & Dashboard", true, fade)) AppGoTo(app, SCREEN_PARK);

    if (UiButton((Rectangle){ col0, by + step * 4, bw, bh }, "Analytics", true, fade)) AppGoTo(app, SCREEN_ANALYTICS);
    if (UiButton((Rectangle){ col1, by + step * 4, bw, bh }, "Settings", true, fade)) AppGoTo(app, SCREEN_SETTINGS);
    if (UiButton((Rectangle){ col0, by + step * 5, bw, bh }, "Chess Arena", true, fade)) AppGoTo(app, SCREEN_CHESS);
    if (UiButton((Rectangle){ col1, by + step * 5, bw, bh }, "Community & Info", true, fade)) AppGoTo(app, SCREEN_COMMUNITY);
    if (UiButton((Rectangle){ col0, by + step * 6, bw, bh }, "Quit Application", true, fade)) app->quit = true;
    if (UiButton((Rectangle){ 1020, 660, 230, 42 },
                  ThemeGet() == THEME_DARK ? "Light Mode  [sun]" : "Dark Mode  [moon]", true, fade))
        ThemeToggle();
    if (UiButton((Rectangle){ 780, 660, 230, 42 },
                  app->focusMode ? "Focus Music: ON  [pause]" : "Focus Music: OFF  [play]", true, fade))
    {
        app->focusMode = !app->focusMode;
        FocusAudioSetEnabled(app->focusMode);
    }
    if (app->focusMode && UiButton((Rectangle){780,610,230,42},"Next focus track",true,fade))
        FocusAudioNextTrack();

    UiDrawText("v0.4  park · dashboard · learn · play", 20, SCREEN_H - 30, 18, Fade(COL_MUTED, fade));
}