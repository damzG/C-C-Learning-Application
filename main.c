// C Square Learn (C^2): entry point.
//
// The game is always drawn at 1280x720 into an off-screen texture, which is then scaled to
// fit the window. So the window can be any size (and is shrunk to fit small or scaled
// displays) and every screen keeps working, mouse included. Press F11 for full screen.

#include <math.h>

#include "raylib.h"
#include "app.h"
#include "theme.h"
#include "ui.h"
#include "panda.h"
#include "focus_audio.h"

// Starts at 1280x720, or smaller when the display cannot show that comfortably.
static void FitWindowToMonitor(void)
{
    int monitor = GetCurrentMonitor();
    float mw = (float)GetMonitorWidth(monitor);
    float mh = (float)GetMonitorHeight(monitor);
    if (mw <= 0 || mh <= 0) return;

    // Leave room for the title bar and taskbar
    float fit = fminf(mw * 0.92f / SCREEN_W, mh * 0.80f / SCREEN_H);
    if (fit > 1.0f) fit = 1.0f;

    int w = (int)(SCREEN_W * fit);
    int h = (int)(SCREEN_H * fit);
    SetWindowSize(w, h);
    SetWindowPosition((int)((mw - w) / 2), (int)((mh - h) / 2.5f));
}

int main(void)
{
    // MSAA smooths circles and rounded corners. Not using FLAG_WINDOW_HIGHDPI on purpose:
    // on a scaled Windows display it makes the window grow to fill the whole screen.
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);

    InitWindow(SCREEN_W, SCREEN_H, "C Square Learn");
    SetExitKey(KEY_NULL);   // Esc is used for "back", not to quit
    SetTargetFPS(60);
    SetWindowMinSize(640, 360);
    FitWindowToMonitor();
    ThemeLoad();
    FocusAudioInit();

    RenderTexture2D target = LoadRenderTexture(SCREEN_W, SCREEN_H);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    AppState app;
    AppInit(&app);

    while (!WindowShouldClose() && !app.quit)
    {
        float dt = GetFrameTime();
        FocusAudioUpdate(app.focusMode);

        if (IsKeyPressed(KEY_F11)) ToggleBorderlessWindowed();

        // Where the 1280x720 picture sits inside the window (centred, black bars if needed)
        float scale = fminf((float)GetScreenWidth() / SCREEN_W, (float)GetScreenHeight() / SCREEN_H);
        float offX = (GetScreenWidth() - SCREEN_W * scale) / 2.0f;
        float offY = (GetScreenHeight() - SCREEN_H * scale) / 2.0f;

        // Every screen reads GetMousePosition(), so map the mouse back to 1280x720 here
        SetMouseOffset((int)-offX, (int)-offY);
        SetMouseScale(1.0f / scale, 1.0f / scale);

        BeginTextureMode(target);
        ClearBackground(COL_BG);
        AppRunScreen(&app, dt);
        UiToastDraw(dt);
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(target.texture,
                       (Rectangle){ 0, 0, (float)SCREEN_W, -(float)SCREEN_H },
                       (Rectangle){ offX, offY, SCREEN_W * scale, SCREEN_H * scale },
                       (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadRenderTexture(target);
    FocusAudioShutdown();
    PandaShutdown();
    UiShutdown();
    CloseWindow();
    return 0;
}