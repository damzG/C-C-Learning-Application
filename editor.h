// A small multi-line code editor for raylib.
#ifndef EDITOR_H
#define EDITOR_H

#include <stdbool.h>
#include "raylib.h"

#define EDITOR_MAX 8192

typedef struct {
    char text[EDITOR_MAX];
    int len;
    int cursor;        // index into text
    int desiredCol;    // remembered column for Up and Down, or -1
    float scrollY;
    Rectangle rect;
} Editor;

void EditorInit(Editor *editor, Rectangle rect);
void EditorSetText(Editor *editor, const char *text);

// active: the editor reacts to the keyboard. Mouse clicks and the wheel always work.
void EditorUpdate(Editor *editor, bool active);
void EditorDraw(const Editor *editor, bool active);

#endif