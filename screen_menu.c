#include "screens.h"
#include "theme.h"
#include "focus_audio.h"
#include "ui.h"
#include "avatar_draw.h"
#include "park_bg.h"
#include "theme.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static float fade;
static int quoteIndex;
static const char *quotes[] = {
    "Small steps every day add up to remarkable progress.",
    "You do not have to be perfect to make progress.",
    "Every expert was once a beginner who kept going.",
    "Take a breath. Solve one problem at a time.",
    "Your effort matters, even on the difficult days."
};

void ScreenMenuEnter(AppState *app)
{
    (void)app;
    fade = 0.0f;
    quoteIndex = rand() % (int)(sizeof(quotes) / sizeof(quotes[0]));
}

void ScreenMenuRun(AppState *app, float dt)
{
    fade = fminf(fade + dt * 2.0f, 1.0f);

    ParkBgDraw(0.45f * fade);
    UiDrawTrack(0.5f * fade);
    UiDrawLogo(SCREEN_W / 2, 130, 100, fade);
    UiDrawCenteredText(quotes[quoteIndex], SCREEN_W / 2, 184, 18, Fade(COL_CYAN, fade));

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

    // Keep the menu in a centered column with consistent gutters. The shorter
    // rows leave breathing room above the persistent controls and footer.
    float bw = 360.0f, bh = 44.0f;
    float gap = 20.0f;
    float col0 = SCREEN_W / 2.0f - bw - gap / 2.0f;
    float col1 = SCREEN_W / 2.0f + gap / 2.0f;
    float by = 218.0f;
    float step = 49.0f;

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
    if (UiButton((Rectangle){ col0, by + step * 6, bw, bh }, "Exam Revision", true, fade)) AppGoTo(app, SCREEN_REVISION);
    if (UiButton((Rectangle){ col1, by + step * 6, bw, bh }, "Video Tutorials", true, fade)) AppGoTo(app, SCREEN_TUTORIALS);
    if (UiButton((Rectangle){ col0, by + step * 7, bw * 2 + gap, bh }, "Quit Application", true, fade)) app->quit = true;
    if (UiButton((Rectangle){ SCREEN_W - 250, SCREEN_H - 58, 230, 40 },
                  ThemeGet() == THEME_DARK ? "Light Mode  [sun]" : "Dark Mode  [moon]", true, fade))
        ThemeToggle();
    if (UiButton((Rectangle){ SCREEN_W - 500, SCREEN_H - 58, 230, 40 },
                  app->focusMode ? "Focus Music: ON  [pause]" : "Focus Music: OFF  [play]", true, fade))
    {
        app->focusMode = !app->focusMode;
        FocusAudioSetEnabled(app->focusMode);
    }
    if (app->focusMode && UiButton((Rectangle){ SCREEN_W - 500, SCREEN_H - 108, 230, 40 },"Next focus track",true,fade))
        FocusAudioNextTrack();

    UiDrawText("v0.4  park · dashboard · learn · play", 20, SCREEN_H - 30, 18, Fade(COL_MUTED, fade));
}
