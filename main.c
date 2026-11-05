// C Square Learn (C^2)
// Milestone 1: window, animated loading screen with the C^2 logo, and a main menu.

#include "raylib.h"
#include <math.h>

#define SCREEN_W 1280
#define SCREEN_H 720

#define LOAD_DURATION 3.0f   // seconds the fake loading bar takes
#define LOAD_HOLD     0.6f   // pause on 100% before the menu appears

typedef enum {
    SCREEN_LOADING,
    SCREEN_MENU,
    SCREEN_LEARN,
    SCREEN_ACCOUNT,
    SCREEN_PUZZLES
} AppScreen;

// Colour palette
static const Color COL_BG      = { 18, 20, 38, 255 };
static const Color COL_PANEL   = { 30, 34, 62, 255 };
static const Color COL_ACCENT  = { 255, 140, 66, 255 };
static const Color COL_CYAN    = { 72, 202, 228, 255 };
static const Color COL_MUTED   = { 150, 156, 190, 255 };

static const char *LOAD_MESSAGES[] = {
    "Warming up the compiler...",
    "Loading pointer lessons...",
    "Allocating heap memory...",
    "Linking the rollercoaster...",
    "Ready to ride!"
};
#define LOAD_MESSAGE_COUNT 5

// ---------------------------------------------------------------------------
// Draws the C^2 logo centred on (cx, cy). "size" is the font size of the C.
// ---------------------------------------------------------------------------
static void DrawLogo(int cx, int cy, int size, float alpha)
{
    int gap = size / 16;
    int cWidth = MeasureText("C", size);
    int twoWidth = MeasureText("2", size / 2);
    int total = cWidth + gap + twoWidth;
    int x = cx - total / 2;
    int y = cy - size / 2;

    DrawText("C", x, y, size, Fade(RAYWHITE, alpha));
    DrawText("2", x + cWidth + gap, y, size / 2, Fade(COL_ACCENT, alpha));
}

// ---------------------------------------------------------------------------
// Rollercoaster track: a smooth wave. Returns the track height at x.
// ---------------------------------------------------------------------------
static float TrackY(float x)
{
    return 600.0f + sinf(x * 0.012f) * 45.0f + sinf(x * 0.031f) * 15.0f;
}

static float TrackSlope(float x)
{
    return cosf(x * 0.012f) * 45.0f * 0.012f + cosf(x * 0.031f) * 15.0f * 0.031f;
}

static void DrawTrack(float alpha)
{
    for (float x = 0; x < SCREEN_W; x += 8.0f)
    {
        Vector2 a = { x, TrackY(x) };
        Vector2 b = { x + 8.0f, TrackY(x + 8.0f) };
        DrawLineEx(a, b, 4.0f, Fade(COL_CYAN, alpha));

        // Support posts every 48 pixels
        if (((int)x % 48) == 0)
        {
            DrawLineEx(a, (Vector2){ x, SCREEN_H }, 2.0f, Fade(COL_MUTED, alpha * 0.4f));
        }
    }
}

static void DrawCart(float x, float alpha)
{
    float y = TrackY(x);
    float angle = atanf(TrackSlope(x)) * RAD2DEG;
    Rectangle body = { x, y - 6.0f, 34.0f, 18.0f };
    Vector2 origin = { 17.0f, 18.0f };

    DrawRectanglePro(body, origin, angle, Fade(COL_ACCENT, alpha));
    DrawCircle((int)x, (int)(y - 2.0f), 3.0f, Fade(RAYWHITE, alpha));
}

// ---------------------------------------------------------------------------
// Simple button. Returns true when clicked.
// ---------------------------------------------------------------------------
static bool DrawButton(Rectangle r, const char *text, bool enabled, float alpha)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = enabled && CheckCollisionPointRec(mouse, r);

    Color fill = !enabled ? Fade(COL_PANEL, 0.6f * alpha)
               : hovered  ? Fade(COL_ACCENT, alpha)
                          : Fade(COL_PANEL, alpha);
    Color textCol = !enabled ? Fade(COL_MUTED, 0.5f * alpha)
                  : hovered  ? Fade(COL_BG, alpha)
                             : Fade(RAYWHITE, alpha);

    DrawRectangleRounded(r, 0.3f, 8, fill);
    DrawRectangleRoundedLinesEx(r, 0.3f, 8, 2.0f, Fade(enabled ? COL_ACCENT : COL_MUTED, alpha * 0.8f));

    int fontSize = 26;
    int tw = MeasureText(text, fontSize);
    DrawText(text, (int)(r.x + (r.width - tw) / 2), (int)(r.y + (r.height - fontSize) / 2), fontSize, textCol);

    return hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

// ---------------------------------------------------------------------------
// Placeholder for screens that are not built yet.
// ---------------------------------------------------------------------------
static bool DrawPlaceholder(const char *title, const char *note)
{
    int tw = MeasureText(title, 48);
    DrawText(title, (SCREEN_W - tw) / 2, 220, 48, RAYWHITE);

    int nw = MeasureText(note, 24);
    DrawText(note, (SCREEN_W - nw) / 2, 300, 24, COL_MUTED);

    Rectangle back = { SCREEN_W / 2.0f - 120, 400, 240, 56 };
    return DrawButton(back, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE);
}

int main(void)
{
    InitWindow(SCREEN_W, SCREEN_H, "C Square Learn");
    SetTargetFPS(60);

    AppScreen screen = SCREEN_LOADING;
    float loadTime = 0.0f;
    float holdTime = 0.0f;
    float menuFade = 0.0f;
    bool quit = false;

    while (!WindowShouldClose() && !quit)
    {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(COL_BG);

        switch (screen)
        {
            case SCREEN_LOADING:
            {
                loadTime += dt;
                float progress = fminf(loadTime / LOAD_DURATION, 1.0f);
                float logoAlpha = fminf(loadTime / 0.8f, 1.0f);

                DrawTrack(logoAlpha);
                DrawCart(60.0f + progress * (SCREEN_W - 120.0f), logoAlpha);

                DrawLogo(SCREEN_W / 2, 220, 140, logoAlpha);

                const char *tagline = "Learn C and C++ by riding the rollercoaster";
                int tagW = MeasureText(tagline, 24);
                DrawText(tagline, (SCREEN_W - tagW) / 2, 320, 24, Fade(COL_MUTED, logoAlpha));

                // Progress bar
                Rectangle bar = { SCREEN_W / 2.0f - 250, 400, 500, 14 };
                DrawRectangleRounded(bar, 0.5f, 8, Fade(COL_PANEL, logoAlpha));
                Rectangle fill = { bar.x, bar.y, bar.width * progress, bar.height };
                if (fill.width > 1.0f)
                {
                    DrawRectangleRounded(fill, 0.5f, 8, COL_ACCENT);
                }

                int msgIndex = (int)(progress * (LOAD_MESSAGE_COUNT - 1) + 0.5f);
                if (msgIndex >= LOAD_MESSAGE_COUNT) msgIndex = LOAD_MESSAGE_COUNT - 1;
                const char *msg = LOAD_MESSAGES[msgIndex];
                int msgW = MeasureText(msg, 20);
                DrawText(msg, (SCREEN_W - msgW) / 2, 430, 20, Fade(COL_MUTED, logoAlpha));

                if (progress >= 1.0f)
                {
                    holdTime += dt;
                    if (holdTime >= LOAD_HOLD)
                    {
                        screen = SCREEN_MENU;
                        menuFade = 0.0f;
                    }
                }
            } break;

            case SCREEN_MENU:
            {
                menuFade = fminf(menuFade + dt * 2.0f, 1.0f);

                DrawTrack(0.5f * menuFade);
                DrawLogo(SCREEN_W / 2, 130, 100, menuFade);

                float bw = 380.0f, bh = 62.0f;
                float bx = (SCREEN_W - bw) / 2.0f;
                float by = 240.0f;
                float step = 80.0f;

                if (DrawButton((Rectangle){ bx, by,            bw, bh }, "Start Learning",  true, menuFade)) screen = SCREEN_LEARN;
                if (DrawButton((Rectangle){ bx, by + step,     bw, bh }, "Create Account",  true, menuFade)) screen = SCREEN_ACCOUNT;
                if (DrawButton((Rectangle){ bx, by + step * 2, bw, bh }, "Puzzle Games",    true, menuFade)) screen = SCREEN_PUZZLES;
                if (DrawButton((Rectangle){ bx, by + step * 3, bw, bh }, "Quit",            true, menuFade)) quit = true;

                DrawText("v0.1  early development", 20, SCREEN_H - 30, 18, Fade(COL_MUTED, menuFade));
            } break;

            case SCREEN_LEARN:
                if (DrawPlaceholder("Rollercoaster Level Map", "Topics and challenges are coming soon.")) screen = SCREEN_MENU;
                break;

            case SCREEN_ACCOUNT:
                if (DrawPlaceholder("Create Account", "Profiles saved to CSV are coming soon.")) screen = SCREEN_MENU;
                break;

            case SCREEN_PUZZLES:
                if (DrawPlaceholder("Puzzle Games", "Code puzzles are coming soon.")) screen = SCREEN_MENU;
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}