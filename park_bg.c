#include "park_bg.h"
#include "theme.h"
#include "ui.h"

#include "raylib.h"
#include <math.h>

#define FW 48
#define BL 18

typedef struct { float x, y, vx, vy, life; Color c; } Firework;
typedef struct { float x, y, phase, size; Color c; } Balloon;

static Firework fw[FW];
static Balloon balloons[BL];
static bool init;

static Color Soft(Color c, float a) { return Fade(c, a); }

static void EnsureInit(void)
{
    if (init) return;
    init = true;
    for (int i = 0; i < FW; i++) fw[i].life = 0;
    static const Color cols[] = {
        {255,99,110,255},{255,211,0,255},{96,165,255,255},{40,214,140,255},{255,140,66,255}
    };
    for (int i = 0; i < BL; i++)
    {
        balloons[i].x = (float)(40 + (i * 97) % (SCREEN_W - 80));
        balloons[i].y = (float)(80 + (i * 53) % 400);
        balloons[i].phase = i * 0.7f;
        balloons[i].size = 14 + (i % 5) * 3;
        balloons[i].c = cols[i % 5];
    }
}

static void SpawnFirework(void)
{
    for (int i = 0; i < FW; i++)
    {
        if (fw[i].life > 0) continue;
        float cx = 100 + GetRandomValue(0, SCREEN_W - 200);
        float cy = 80 + GetRandomValue(0, 220);
        for (int k = 0; k < 10 && i + k < FW; k++)
        {
            float ang = k * 0.628f + GetTime();
            fw[i + k].x = cx; fw[i + k].y = cy;
            fw[i + k].vx = cosf(ang) * (2.0f + k * 0.15f);
            fw[i + k].vy = sinf(ang) * (2.0f + k * 0.15f);
            fw[i + k].life = 1.0f;
            fw[i + k].c = (Color){ (unsigned char)GetRandomValue(120, 255),
                                    (unsigned char)GetRandomValue(80, 255),
                                    (unsigned char)GetRandomValue(80, 255), 255 };
        }
        return;
    }
}

static void DrawFerris(float cx, float cy, float R, float alpha)
{
    float t = (float)GetTime() * 0.35f;
    DrawCircleLines((int)cx, (int)cy, (int)R, Soft(COL_MUTED, alpha * 0.7f));
    DrawCircleLines((int)cx, (int)cy, (int)(R * 0.55f), Soft(COL_MUTED, alpha * 0.4f));
    for (int i = 0; i < 8; i++)
    {
        float a = t + i * (PI * 2.0f / 8.0f);
        float x = cx + cosf(a) * R;
        float y = cy + sinf(a) * R;
        DrawLineEx((Vector2){ cx, cy }, (Vector2){ x, y }, 2.0f, Soft(COL_MUTED, alpha * 0.5f));
        DrawRectangleRounded((Rectangle){ x - 16, y - 12, 32, 24 }, 0.3f, 4, Soft(COL_ACCENT, alpha * 0.85f));
    }
    DrawCircle((int)cx, (int)cy, 10, Soft(COL_CYAN, alpha));
}

static void DrawClown(float x, float y, float alpha)
{
    DrawCircle((int)x, (int)y, 22, Soft((Color){255, 230, 200, 255}, alpha));
    DrawCircle((int)(x - 8), (int)(y - 4), 4, Soft(BLACK, alpha));
    DrawCircle((int)(x + 8), (int)(y - 4), 4, Soft(BLACK, alpha));
    DrawCircle((int)x, (int)(y + 6), 6, Soft(COL_ERROR, alpha)); // nose
    DrawEllipse((int)x, (int)(y + 28), 18, 22, Soft(COL_CYAN, alpha)); // body
    // laptop
    DrawRectangleRounded((Rectangle){ x + 18, y + 18, 34, 22 }, 0.15f, 4, Soft(COL_PANEL, alpha));
    UiDrawMono("</>", (int)(x + 24), (int)(y + 22), 14, Soft(COL_DONE, alpha));
}

static void DrawElephant(float x, float y, float alpha)
{
    DrawEllipse((int)x, (int)y, 34, 22, Soft(COL_MUTED, alpha * 0.9f));
    DrawCircle((int)(x - 28), (int)(y - 4), 16, Soft(COL_MUTED, alpha * 0.9f));
    DrawRectangleRounded((Rectangle){ x - 46, y + 2, 10, 28 }, 0.4f, 4, Soft(COL_MUTED, alpha * 0.85f)); // trunk
    DrawCircle((int)(x - 34), (int)(y - 10), 5, Soft(COL_PANEL, alpha));
}

void ParkBgDraw(float alpha)
{
    EnsureInit();
    float t = (float)GetTime();

    // Soft sky gradient bands
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Soft(COL_BG, alpha));
    DrawRectangleGradientV(0, 0, SCREEN_W, 260, Soft((Color){28, 40, 90, 255}, alpha * 0.55f), Soft(COL_BG, 0));

    // Infinite scrolling track + carts
    float scroll = fmodf(t * 90.0f, (float)SCREEN_W);
    for (int k = -1; k <= 1; k++)
    {
        float ox = k * SCREEN_W - scroll;
        for (int x = 0; x < SCREEN_W; x += 8)
        {
            float px = ox + x;
            float y = 560.0f + sinf(px * 0.012f) * 28.0f;
            DrawCircle((int)px, (int)y, 3, Soft(COL_MUTED, alpha * 0.35f));
        }
        for (int c = 0; c < 4; c++)
        {
            float cx = ox + 80 + c * 280.0f;
            float cy = 560.0f + sinf(cx * 0.012f) * 28.0f;
            float ang = cosf(cx * 0.012f) * 12.0f;
            UiDrawCartAt(cx, cy - 18, ang, alpha * 0.55f);
        }
    }

    DrawFerris(1080, 210, 110, alpha * 0.9f);

    // Circus tent
    DrawTriangle((Vector2){ 80, 220 }, (Vector2){ 200, 220 }, (Vector2){ 140, 110 }, Soft(COL_ERROR, alpha * 0.55f));
    DrawRectangle(90, 220, 100, 50, Soft(COL_PANEL, alpha * 0.5f));

    DrawClown(260 + sinf(t) * 8, 470, alpha * 0.85f);
    DrawElephant(420, 500, alpha * 0.7f);
    DrawElephant(520, 510, alpha * 0.55f);

    // Balloons
    for (int i = 0; i < BL; i++)
    {
        float bx = balloons[i].x + sinf(t * 0.6f + balloons[i].phase) * 12.0f;
        float by = balloons[i].y - fmodf(t * 18.0f + i * 40.0f, 520.0f);
        if (by < 20) by += 520;
        float s = balloons[i].size;
        DrawCircle((int)bx, (int)by, (int)s, Soft(balloons[i].c, alpha * 0.75f));
        DrawLineEx((Vector2){ bx, by + s }, (Vector2){ bx + 4, by + s + 28 }, 1.5f, Soft(COL_MUTED, alpha * 0.5f));
    }

    // Fireworks
    if (GetRandomValue(0, 40) == 0) SpawnFirework();
    for (int i = 0; i < FW; i++)
    {
        if (fw[i].life <= 0) continue;
        fw[i].life -= 0.016f;
        fw[i].x += fw[i].vx;
        fw[i].y += fw[i].vy;
        fw[i].vy += 0.04f;
        DrawCircle((int)fw[i].x, (int)fw[i].y, 3, Soft(fw[i].c, fw[i].life * alpha));
    }

    // Ground wash so UI stays readable
    DrawRectangleGradientV(0, SCREEN_H - 180, SCREEN_W, 180, Soft(COL_BG, 0), Soft(COL_BG, alpha * 0.85f));
}
