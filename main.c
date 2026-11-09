// C Square Learn (C^2): entry point. All screens live in src/screens.

#include "raylib.h"
#include "app.h"
#include "theme.h"
#include "ui.h"

int main(void)
{
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

    CloseWindow();
    return 0;
}