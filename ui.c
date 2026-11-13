#include "ui.h"
#include "theme.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Fonts
//
// raylib's built in font is a tiny bitmap font. Drawing it at 24 or 40 pixels just
// stretches the pixels, which is why text looks blocky. Here every size is rasterised
// from a TrueType font at exactly the size needed (and at the screen's DPI scale on
// high resolution displays), so the text stays sharp.
// ---------------------------------------------------------------------------
#define FONT_CACHE_MAX 64
#define BOLD_FROM_SIZE 28

// A TrueType font looks smaller than raylib's built in font at the same size number,
// because the size includes space above and below the letters. This scale keeps the
// text the size the layouts were designed for. Lower it if text feels too large.
#define TEXT_SCALE 1.2f

typedef enum { STYLE_REGULAR, STYLE_BOLD, STYLE_MONO } FontStyle;

typedef struct {
    int size;
    FontStyle style;
    Font font;
    float spacing;
} CachedFont;

static CachedFont fontCache[FONT_CACHE_MAX];
static int fontCacheCount = 0;
static CachedFont fallbackFont;
static bool fallbackWarned = false;

static const char *FONT_FILES[] = {
    "assets/fonts/Poppins-Regular.ttf",
    "assets/fonts/Poppins-SemiBold.ttf",
    "assets/fonts/IBMPlexMono-Regular.ttf"
};

static float DpiScale(void)
{
    Vector2 scale = GetWindowScaleDPI();
    return scale.x > 1.0f ? scale.x : 1.0f;
}

// Looks next to the working directory first, then next to the executable.
static const char *FindFontFile(FontStyle style)
{
    const char *relative = FONT_FILES[style];
    if (FileExists(relative)) return relative;

    const char *besideExe = TextFormat("%s%s", GetApplicationDirectory(), relative);
    if (FileExists(besideExe)) return besideExe;
    return NULL;
}

static const CachedFont *GetCachedFont(int size, FontStyle style)
{
    for (int i = 0; i < fontCacheCount; i++)
    {
        if (fontCache[i].size == size && fontCache[i].style == style) return &fontCache[i];
    }

    if (fontCacheCount < FONT_CACHE_MAX)
    {
        const char *path = FindFontFile(style);
        if (path != NULL)
        {
            int pixels = (int)((float)size * DpiScale() + 0.5f);
            Font font = LoadFontEx(path, pixels, NULL, 0);

            if (font.texture.id != 0 && font.glyphCount > 0)
            {
                SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
                CachedFont *entry = &fontCache[fontCacheCount++];
                entry->size = size;
                entry->style = style;
                entry->font = font;
                entry->spacing = 0.0f;
                return entry;
            }
        }
    }

    // Font file missing: fall back to raylib's built in font so the app still runs
    if (!fallbackWarned)
    {
        TraceLog(LOG_WARNING, "UI font not found. Copy the assets folder next to the executable.");
        fallbackWarned = true;
    }
    fallbackFont.font = GetFontDefault();
    fallbackFont.size = size;
    fallbackFont.spacing = (float)size / 10.0f;
    return &fallbackFont;
}

static int ScaledSize(int size)
{
    return (int)((float)size * TEXT_SCALE + 0.5f);
}

static void DrawWithStyle(const char *text, int x, int y, int size, Color color, FontStyle style)
{
    int drawSize = ScaledSize(size);
    const CachedFont *cf = GetCachedFont(drawSize, style);
    DrawTextEx(cf->font, text, (Vector2){ (float)x, (float)y }, (float)drawSize, cf->spacing, color);
}

static int MeasureWithStyle(const char *text, int size, FontStyle style)
{
    int drawSize = ScaledSize(size);
    const CachedFont *cf = GetCachedFont(drawSize, style);
    return (int)(MeasureTextEx(cf->font, text, (float)drawSize, cf->spacing).x + 0.5f);
}

static FontStyle StyleForSize(int size)
{
    return size >= BOLD_FROM_SIZE ? STYLE_BOLD : STYLE_REGULAR;
}

void UiDrawText(const char *text, int x, int y, int size, Color color)
{
    DrawWithStyle(text, x, y, size, color, StyleForSize(size));
}

int UiMeasureText(const char *text, int size)
{
    return MeasureWithStyle(text, size, StyleForSize(size));
}

void UiDrawMono(const char *text, int x, int y, int size, Color color)
{
    DrawWithStyle(text, x, y, size, color, STYLE_MONO);
}

int UiMeasureMono(const char *text, int size)
{
    return MeasureWithStyle(text, size, STYLE_MONO);
}

void UiShutdown(void)
{
    for (int i = 0; i < fontCacheCount; i++) UnloadFont(fontCache[i].font);
    fontCacheCount = 0;
}

// ---------------------------------------------------------------------------
// Logo and rollercoaster track
// ---------------------------------------------------------------------------
void UiDrawLogo(int cx, int cy, int size, float alpha)
{
    int gap = size / 16;
    int cWidth = UiMeasureText("C", size);
    int twoWidth = UiMeasureText("2", size / 2);
    int total = cWidth + gap + twoWidth;
    int x = cx - total / 2;
    int y = cy - size / 2;

    UiDrawText("C", x, y, size, Fade(RAYWHITE, alpha));
    UiDrawText("2", x + cWidth + gap, y, size / 2, Fade(COL_ACCENT, alpha));
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
    UiDrawText(text, cx - UiMeasureText(text, size) / 2, y, size, color);
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

        if (line[0] && UiMeasureText(test, size) > (int)area.width)
        {
            UiDrawText(line, (int)area.x, (int)y, size, color);
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
        UiDrawText(line, (int)area.x, (int)y, size, color);
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
            UiDrawMono(line, (int)area.x + 18, (int)y, 19, (Color){ 214, 226, 255, 255 });
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
static bool DrawButton(Rectangle r, const char *text, bool enabled, float alpha, bool primary)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = enabled && CheckCollisionPointRec(mouse, r);

    Color fill, border, textCol;
    if (primary)
    {
        fill = !enabled ? Fade(COL_MUTED, 0.3f * alpha)
             : hovered  ? Fade((Color){ 255, 226, 80, 255 }, alpha)
                        : Fade(COL_ACCENT, alpha);
        border = fill;
        textCol = !enabled ? Fade(COL_BG, 0.6f * alpha) : Fade(COL_BG, alpha);
    }
    else
    {
        fill = hovered ? Fade((Color){ 44, 52, 92, 255 }, alpha) : Fade(COL_PANEL, 0.0f);
        border = !enabled ? Fade(COL_MUTED, 0.25f * alpha)
               : hovered  ? Fade(COL_ACCENT, alpha)
                          : Fade(COL_MUTED, 0.55f * alpha);
        textCol = !enabled ? Fade(COL_MUTED, 0.5f * alpha) : Fade(RAYWHITE, alpha);
    }

    DrawRectangleRounded(r, 0.22f, 10, fill);
    DrawRectangleRoundedLinesEx(r, 0.22f, 10, 2.0f, border);

    const int fontSize = 22;
    int tw = MeasureWithStyle(text, fontSize, STYLE_BOLD);
    DrawWithStyle(text, (int)(r.x + (r.width - tw) / 2), (int)(r.y + (r.height - fontSize * TEXT_SCALE) / 2) - 3, fontSize, textCol, STYLE_BOLD);

    return hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

bool UiButton(Rectangle r, const char *text, bool enabled, float alpha)
{
    return DrawButton(r, text, enabled, alpha, false);
}

bool UiButtonPrimary(Rectangle r, const char *text, bool enabled, float alpha)
{
    return DrawButton(r, text, enabled, alpha, true);
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

    UiDrawText(box->label, x, (int)box->rect.y - 24, 18, COL_MUTED);
    DrawRectangleRounded(box->rect, 0.2f, 8, COL_PANEL);
    DrawRectangleRoundedLinesEx(box->rect, 0.2f, 8, 2.0f, active ? COL_ACCENT : Fade(COL_MUTED, 0.5f));

    // Show the end of the text if it is too long to fit
    const char *shown = box->text;
    while (*shown && UiMeasureText(shown, size) > maxW) shown++;

    int ty = (int)(box->rect.y + (box->rect.height - size) / 2);
    if (box->len == 0) UiDrawText(box->placeholder, x + pad, ty, size, Fade(COL_MUTED, 0.6f));
    else               UiDrawText(shown, x + pad, ty, size, RAYWHITE);

    if (active && ((int)(GetTime() * 2.0) % 2 == 0))
    {
        int cx = x + pad + UiMeasureText(shown, size) + 2;
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
    int w = UiMeasureText(toastMessage, 22) + 48;
    Rectangle r = { (SCREEN_W - w) / 2.0f, SCREEN_H - 90.0f, (float)w, 48.0f };

    DrawRectangleRounded(r, 0.4f, 8, Fade(COL_PANEL, alpha));
    DrawRectangleRoundedLinesEx(r, 0.4f, 8, 2.0f, Fade(COL_ACCENT, alpha));
    UiDrawText(toastMessage, (int)r.x + 24, (int)r.y + 13, 22, Fade(RAYWHITE, alpha));
}