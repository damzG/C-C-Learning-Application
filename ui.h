// Shared drawing helpers and widgets.
#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include "raylib.h"

#define TEXTBOX_LEN 64

typedef struct {
    Rectangle rect;
    char text[TEXTBOX_LEN];
    int len;
    const char *label;
    const char *placeholder;
} UiTextBox;

// ---- Text. Drop-in replacements for raylib's DrawText and MeasureText that use the
// project fonts (Poppins for text, IBM Plex Mono for code). Text of size 28 and above
// is drawn semi-bold automatically so headings stand out.
void UiDrawText(const char *text, int x, int y, int size, Color color);
int UiMeasureText(const char *text, int size);
void UiDrawMono(const char *text, int x, int y, int size, Color color);
int UiMeasureMono(const char *text, int size);

// Call once before CloseWindow to free the loaded fonts.
void UiShutdown(void);

void UiDrawLogo(int cx, int cy, int size, float alpha);
void UiDrawTrack(float alpha);
void UiDrawCart(float x, float alpha);
void UiDrawCartAt(float x, float y, float angleDeg, float alpha);

void UiDrawCenteredText(const char *text, int cx, int y, int size, Color color);
// Returns the height of the text that was drawn.
float UiDrawWrappedText(const char *text, Rectangle area, int size, Color color);

int UiCodeLineCount(const char *code);
void UiDrawCodeBlock(const char *code, Rectangle area);

bool UiButton(Rectangle rect, const char *text, bool enabled, float alpha);
// The main call to action on a screen: solid accent colour.
bool UiButtonPrimary(Rectangle rect, const char *text, bool enabled, float alpha);

void UiTextBoxInit(UiTextBox *box, Rectangle rect, const char *label, const char *placeholder);
void UiTextBoxClear(UiTextBox *box);
void UiTextBoxUpdate(UiTextBox *box);
void UiTextBoxDraw(const UiTextBox *box, bool active);

void UiToast(const char *message);
void UiToastDraw(float dt);

#endif