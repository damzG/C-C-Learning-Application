// C Square Learn (C^2): entry point. All screens live in src/screens.

#include "raylib.h"
#include "app.h"
#include "theme.h"
#include "ui.h"

int main(void)
{
    // Smooth edges on circles, lines and rounded corners, and sharp rendering on
    // high resolution (scaled) displays. These must be set before InitWindow.
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);

    InitWindow(SCREEN_W, SCREEN_H, "C Square Learn");
    SetExitKey(KEY_NULL);   // Esc is used for "back", not to quit
    SetTargetFPS(60);

    AppState app;
    AppInit(&app);

    while (!WindowShouldClose() && !app.quit)
    {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(COL_BG);
        AppRunScreen(&app, dt);
        UiToastDraw(dt);
        EndDrawing();
    }

    UiShutdown();
    CloseWindow();
    return 0;
}