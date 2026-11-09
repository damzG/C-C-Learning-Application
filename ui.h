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

void UiTextBoxInit(UiTextBox *box, Rectangle rect, const char *label, const char *placeholder);
void UiTextBoxClear(UiTextBox *box);
void UiTextBoxUpdate(UiTextBox *box);
void UiTextBoxDraw(const UiTextBox *box, bool active);

void UiToast(const char *message);
void UiToastDraw(float dt);

#endif