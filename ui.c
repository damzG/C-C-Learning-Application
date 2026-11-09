#include "ui.h"
#include "theme.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Logo and rollercoaster track
// ---------------------------------------------------------------------------
void UiDrawLogo(int cx, int cy, int size, float alpha)
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

static float TrackY(float x)
{
    return 600.0f + sinf(x * 0.012f) * 45.0f + sinf(x * 0.031f) * 15.0f;
}

static float TrackSlope(float x)
{
    return cosf(x * 0.012f) * 45.0f * 0.012f + cosf(x * 0.031f) * 15.0f * 0.031f;
}

void UiDrawTrack(float alpha)
{
    for (float x = 0; x < SCREEN_W; x += 8.0f)
    {
        Vector2 a = { x, TrackY(x) };
        Vector2 b = { x + 8.0f, TrackY(x + 8.0f) };
        DrawLineEx(a, b, 4.0f, Fade(COL_CYAN, alpha));

        if (((int)x % 48) == 0)
        {
            DrawLineEx(a, (Vector2){ x, SCREEN_H }, 2.0f, Fade(COL_MUTED, alpha * 0.4f));
        }
    }
}

void UiDrawCartAt(float x, float y, float angleDeg, float alpha)
{
    Rectangle body = { x, y - 6.0f, 34.0f, 18.0f };
    Vector2 origin = { 17.0f, 18.0f };

    DrawRectanglePro(body, origin, angleDeg, Fade(COL_ACCENT, alpha));
    DrawCircle((int)x, (int)(y - 2.0f), 3.0f, Fade(RAYWHITE, alpha));
}

void UiDrawCart(float x, float alpha)
{
    UiDrawCartAt(x, TrackY(x), atanf(TrackSlope(x)) * RAD2DEG, alpha);
}

// ---------------------------------------------------------------------------
// Text helpers
// ---------------------------------------------------------------------------
void UiDrawCenteredText(const char *text, int cx, int y, int size, Color color)
{
    DrawText(text, cx - MeasureText(text, size) / 2, y, size, color);
}

float UiDrawWrappedText(const char *text, Rectangle area, int size, Color color)
{
    char line[256] = "";
    char word[64];
    float y = area.y;
    const char *p = text;

    while (*p)
    {
        while (*p == ' ') p++;

        int wl = 0;
        while (*p && *p != ' ' && wl < 63) word[wl++] = *p++;
        word[wl] = '\0';
        if (wl == 0) break;

        char test[320];
        snprintf(test, sizeof(test), "%s%s%s", line, line[0] ? " " : "", word);

        if (line[0] && MeasureText(test, size) > (int)area.width)
        {
            DrawText(line, (int)area.x, (int)y, size, color);
            y += size * 1.4f;
            snprintf(line, sizeof(line), "%s", word);
        }
        else
        {
            snprintf(line, sizeof(line), "%s", test);
        }
    }

    if (line[0])
    {
        DrawText(line, (int)area.x, (int)y, size, color);
        y += size * 1.4f;
    }
    return y - area.y;
}

int UiCodeLineCount(const char *code)
{
    if (code == NULL || code[0] == '\0') return 0;

    int lines = 1;
    for (const char *p = code; *p; p++)
    {
        if (*p == '\n') lines++;
    }
    return lines;
}

void UiDrawCodeBlock(const char *code, Rectangle area)
{
    DrawRectangleRounded(area, 0.08f, 8, (Color){ 10, 12, 24, 255 });
    DrawRectangleRoundedLinesEx(area, 0.08f, 8, 1.0f, Fade(COL_MUTED, 0.4f));

    char line[128];
    int len = 0;
    float y = area.y + 12.0f;

    for (const char *p = code; ; p++)
    {
        if (*p == '\n' || *p == '\0')
        {
            line[len] = '\0';
            DrawText(line, (int)area.x + 18, (int)y, 20, (Color){ 200, 230, 200, 255 });
            y += 26.0f;
            len = 0;
            if (*p == '\0') break;
        }
        else if (len < (int)sizeof(line) - 1)
        {
            line[len++] = *p;
        }
    }
}

// ---------------------------------------------------------------------------
// Button
// ---------------------------------------------------------------------------
bool UiButton(Rectangle r, const char *text, bool enabled, float alpha)
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

    int fontSize = 24;
    int tw = MeasureText(text, fontSize);
    DrawText(text, (int)(r.x + (r.width - tw) / 2), (int)(r.y + (r.height - fontSize) / 2), fontSize, textCol);

    return hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

// ---------------------------------------------------------------------------
// Text box
// ---------------------------------------------------------------------------
void UiTextBoxInit(UiTextBox *box, Rectangle rect, const char *label, const char *placeholder)
{
    box->rect = rect;
    box->label = label;
    box->placeholder = placeholder;
    UiTextBoxClear(box);
}

void UiTextBoxClear(UiTextBox *box)
{
    box->text[0] = '\0';
    box->len = 0;
}

void UiTextBoxUpdate(UiTextBox *box)
{
    int c;
    while ((c = GetCharPressed()) > 0)
    {
        if (c >= 32 && c <= 126 && box->len < TEXTBOX_LEN - 1)
        {
            box->text[box->len++] = (char)c;
            box->text[box->len] = '\0';
        }
    }

    if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && box->len > 0)
    {
        box->text[--box->len] = '\0';
    }
}

void UiTextBoxDraw(const UiTextBox *box, bool active)
{
    const int size = 22;
    const int pad = 14;
    int x = (int)box->rect.x;
    int maxW = (int)box->rect.width - 2 * pad;

    DrawText(box->label, x, (int)box->rect.y - 24, 18, COL_MUTED);
    DrawRectangleRounded(box->rect, 0.2f, 8, COL_PANEL);
    DrawRectangleRoundedLinesEx(box->rect, 0.2f, 8, 2.0f, active ? COL_ACCENT : Fade(COL_MUTED, 0.5f));

    // Show the end of the text if it is too long to fit
    const char *shown = box->text;
    while (*shown && MeasureText(shown, size) > maxW) shown++;

    int ty = (int)(box->rect.y + (box->rect.height - size) / 2);
    if (box->len == 0) DrawText(box->placeholder, x + pad, ty, size, Fade(COL_MUTED, 0.6f));
    else               DrawText(shown, x + pad, ty, size, RAYWHITE);

    if (active && ((int)(GetTime() * 2.0) % 2 == 0))
    {
        int cx = x + pad + MeasureText(shown, size) + 2;
        DrawRectangle(cx, ty, 2, size, COL_ACCENT);
    }
}

// ---------------------------------------------------------------------------
// Toast message
// ---------------------------------------------------------------------------
static char toastMessage[96];
static float toastTime = 0.0f;

void UiToast(const char *message)
{
    snprintf(toastMessage, sizeof(toastMessage), "%s", message);
    toastTime = 2.5f;
}

void UiToastDraw(float dt)
{
    if (toastTime <= 0.0f) return;
    toastTime -= dt;

    float alpha = toastTime < 0.5f ? (toastTime > 0.0f ? toastTime / 0.5f : 0.0f) : 1.0f;
    int w = MeasureText(toastMessage, 22) + 48;
    Rectangle r = { (SCREEN_W - w) / 2.0f, SCREEN_H - 90.0f, (float)w, 48.0f };

    DrawRectangleRounded(r, 0.4f, 8, Fade(COL_PANEL, alpha));
    DrawRectangleRoundedLinesEx(r, 0.4f, 8, 2.0f, Fade(COL_ACCENT, alpha));
    DrawText(toastMessage, (int)r.x + 24, (int)r.y + 13, 22, Fade(RAYWHITE, alpha));
}