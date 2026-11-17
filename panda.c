#include "panda.h"

#include <math.h>

static Texture2D picture;
static bool pictureTried = false;
static bool pictureOk = false;

static void TryLoadPicture(void)
{
    if (pictureTried) return;
    pictureTried = true;

    static const char *PATHS[] = { "assets/panda.png", "assets/images/panda.png", "assets/panda/panda.png" };
    for (int i = 0; i < 3; i++)
    {
        const char *path = PATHS[i];
        if (!FileExists(path)) path = TextFormat("%s%s", GetApplicationDirectory(), PATHS[i]);
        if (!FileExists(path)) continue;

        picture = LoadTexture(path);
        if (picture.id != 0)
        {
            SetTextureFilter(picture, TEXTURE_FILTER_BILINEAR);
            pictureOk = true;
            TraceLog(LOG_INFO, "Panda picture loaded from %s", path);
            return;
        }
    }
}

void PandaShutdown(void)
{
    if (pictureOk) UnloadTexture(picture);
    pictureOk = false;
}

void PandaDraw(int x, int y, int size, PandaMood mood, float alpha)
{
    TryLoadPicture();

    if (pictureOk)
    {
        Rectangle src = { 0, 0, (float)picture.width, (float)picture.height };
        Rectangle dst = { (float)x, (float)y, (float)size, (float)size };
        DrawTexturePro(picture, src, dst, (Vector2){ 0, 0 }, 0.0f, Fade(WHITE, alpha));
        return;
    }

    float s = (float)size;
    float t = (float)GetTime();
    Color white = Fade((Color){ 250, 250, 252, 255 }, alpha);
    Color black = Fade((Color){ 28, 30, 40, 255 }, alpha);
    Color blush = Fade((Color){ 255, 150, 160, 255 }, alpha * 0.8f);

    // A gentle bob while it is thinking or happy
    float bob = (mood == PANDA_HAPPY) ? -fabsf(sinf(t * 7.0f)) * s * 0.05f : (mood == PANDA_THINK ? sinf(t * 3.0f) * s * 0.02f : 0.0f);

    Vector2 c = { x + s * 0.5f, y + s * 0.46f + bob };
    float R = s * 0.33f;

    // Body and arms
    DrawEllipse((int)c.x, (int)(y + s * 0.90f + bob), s * 0.27f, s * 0.15f, white);
    DrawCircle((int)(c.x - s * 0.27f), (int)(y + s * 0.86f + bob), s * 0.075f, black);
    DrawCircle((int)(c.x + s * 0.27f), (int)(y + s * 0.86f + bob), s * 0.075f, black);

    // Ears, head
    DrawCircleV((Vector2){ c.x - R * 0.80f, c.y - R * 0.82f }, s * 0.115f, black);
    DrawCircleV((Vector2){ c.x + R * 0.80f, c.y - R * 0.82f }, s * 0.115f, black);
    DrawCircleV(c, R, white);
    DrawCircleLines((int)c.x, (int)c.y, R, Fade(black, 0.25f));

    // Eye patches
    float px = R * 0.42f, py = c.y - R * 0.05f;
    DrawEllipse((int)(c.x - px), (int)py, R * 0.27f, R * 0.35f, black);
    DrawEllipse((int)(c.x + px), (int)py, R * 0.27f, R * 0.35f, black);

    // Eyes
    bool blink = fmodf(t, 3.6f) < 0.12f;
    for (int side = -1; side <= 1; side += 2)
    {
        float ex = c.x + side * px;
        if (mood == PANDA_HAPPY)
        {
            // Closed happy arcs
            DrawRing((Vector2){ ex, py + R * 0.07f }, R * 0.10f, R * 0.15f, 200, 340, 10, white);
        }
        else if (blink)
        {
            DrawLineEx((Vector2){ ex - R * 0.11f, py }, (Vector2){ ex + R * 0.11f, py }, s * 0.02f, white);
        }
        else
        {
            float lookX = (mood == PANDA_THINK) ? R * 0.04f : 0.0f;
            float lookY = (mood == PANDA_THINK) ? -R * 0.05f : (mood == PANDA_SAD ? R * 0.04f : 0.0f);
            DrawCircleV((Vector2){ ex, py }, R * 0.13f, white);
            DrawCircleV((Vector2){ ex + lookX, py + lookY }, R * 0.07f, black);
        }
    }

    // Sad eyebrows
    if (mood == PANDA_SAD)
    {
        DrawLineEx((Vector2){ c.x - px - R * 0.2f, py - R * 0.42f }, (Vector2){ c.x - px + R * 0.15f, py - R * 0.30f }, s * 0.022f, black);
        DrawLineEx((Vector2){ c.x + px + R * 0.2f, py - R * 0.42f }, (Vector2){ c.x + px - R * 0.15f, py - R * 0.30f }, s * 0.022f, black);
    }

    // Nose and mouth
    DrawEllipse((int)c.x, (int)(c.y + R * 0.26f), R * 0.13f, R * 0.09f, black);
    Vector2 m = { c.x, c.y + R * 0.38f };
    if (mood == PANDA_HAPPY)      DrawRing((Vector2){ m.x, m.y - R * 0.02f }, R * 0.14f, R * 0.19f, 15, 165, 12, black);
    else if (mood == PANDA_SAD)   DrawRing((Vector2){ m.x, m.y + R * 0.24f }, R * 0.12f, R * 0.17f, 200, 340, 12, black);
    else if (mood == PANDA_THINK) DrawCircleLines((int)(m.x + R * 0.1f), (int)(m.y + R * 0.1f), R * 0.07f, black);
    else                          DrawRing((Vector2){ m.x, m.y - R * 0.02f }, R * 0.10f, R * 0.14f, 25, 155, 10, black);

    if (mood == PANDA_HAPPY)
    {
        DrawCircleV((Vector2){ c.x - R * 0.72f, c.y + R * 0.30f }, R * 0.11f, blush);
        DrawCircleV((Vector2){ c.x + R * 0.72f, c.y + R * 0.30f }, R * 0.11f, blush);
    }

    // A small light bulb while a hint is showing
    if (mood == PANDA_THINK)
    {
        Vector2 b = { x + s * 0.88f, y + s * 0.12f };
        DrawCircleV(b, s * 0.075f, Fade((Color){ 255, 211, 0, 255 }, alpha));
        DrawRectangle((int)(b.x - s * 0.03f), (int)(b.y + s * 0.07f), (int)(s * 0.06f), (int)(s * 0.035f), Fade((Color){ 160, 168, 200, 255 }, alpha));
    }
}
