#include "screens.h"
#include "theme.h"
#include "ui.h"

#include <math.h>

#define LOAD_DURATION 3.0f   // seconds the loading bar takes
#define LOAD_HOLD     0.6f   // pause on 100% before the menu appears

static const char *LOAD_MESSAGES[] = {
    "Warming up the compiler...",
    "Loading pointer lessons...",
    "Allocating heap memory...",
    "Linking the rollercoaster...",
    "Ready to ride!"
};
#define LOAD_MESSAGE_COUNT 5

static float loadTime;
static float holdTime;

void ScreenLoadingEnter(AppState *app)
{
    (void)app;
    loadTime = 0.0f;
    holdTime = 0.0f;
}

void ScreenLoadingRun(AppState *app, float dt)
{
    loadTime += dt;
    float progress = fminf(loadTime / LOAD_DURATION, 1.0f);
    float logoAlpha = fminf(loadTime / 0.8f, 1.0f);

    UiDrawTrack(logoAlpha);
    UiDrawCart(60.0f + progress * (SCREEN_W - 120.0f), logoAlpha);

    UiDrawLogo(SCREEN_W / 2, 220, 140, logoAlpha);
    UiDrawCenteredText("Learn C and C++ by riding the rollercoaster", SCREEN_W / 2, 320, 24, Fade(COL_MUTED, logoAlpha));

    Rectangle bar = { SCREEN_W / 2.0f - 250, 400, 500, 14 };
    DrawRectangleRounded(bar, 0.5f, 8, Fade(COL_PANEL, logoAlpha));
    Rectangle fill = { bar.x, bar.y, bar.width * progress, bar.height };
    if (fill.width > 1.0f) DrawRectangleRounded(fill, 0.5f, 8, COL_ACCENT);

    int msgIndex = (int)(progress * (LOAD_MESSAGE_COUNT - 1) + 0.5f);
    if (msgIndex >= LOAD_MESSAGE_COUNT) msgIndex = LOAD_MESSAGE_COUNT - 1;
    UiDrawCenteredText(LOAD_MESSAGES[msgIndex], SCREEN_W / 2, 430, 20, Fade(COL_MUTED, logoAlpha));

    if (progress >= 1.0f)
    {
        holdTime += dt;
        if (holdTime >= LOAD_HOLD) AppGoTo(app, SCREEN_MENU);
    }
}