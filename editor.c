#include "editor.h"
#include "theme.h"
#include "ui.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define LINE_H    28
#define FONT_SIZE 18
#define GUTTER    58
#define PAD       12
#define INDENT    4

// ---------------------------------------------------------------------------
// Text helpers
// ---------------------------------------------------------------------------
static int LineCount(const Editor *e)
{
    int n = 1;
    for (int i = 0; i < e->len; i++)
    {
        if (e->text[i] == '\n') n++;
    }
    return n;
}

static int LineStart(const Editor *e, int line)
{
    int idx = 0;
    for (int l = 0; l < line; l++)
    {
        while (idx < e->len && e->text[idx] != '\n') idx++;
        if (idx < e->len) idx++;
    }
    return idx;
}

static int LineLength(const Editor *e, int line)
{
    int start = LineStart(e, line);
    int n = 0;
    while (start + n < e->len && e->text[start + n] != '\n') n++;
    return n;
}

static void CursorPos(const Editor *e, int *line, int *col)
{
    int l = 0, start = 0;
    for (int i = 0; i < e->cursor; i++)
    {
        if (e->text[i] == '\n')
        {
            l++;
            start = i + 1;
        }
    }
    *line = l;
    *col = e->cursor - start;
}

static void Insert(Editor *e, const char *s, int n)
{
    if (e->len + n >= EDITOR_MAX) return;
    memmove(e->text + e->cursor + n, e->text + e->cursor, (size_t)(e->len - e->cursor + 1));
    memcpy(e->text + e->cursor, s, (size_t)n);
    e->len += n;
    e->cursor += n;
}

static void DeleteAt(Editor *e, int from, int count)
{
    if (from < 0 || count <= 0 || from + count > e->len) return;
    memmove(e->text + from, e->text + from + count, (size_t)(e->len - from - count + 1));
    e->len -= count;
}

static float ViewHeight(const Editor *e)
{
    return e->rect.height - 2.0f * PAD;
}

static float MaxScroll(const Editor *e)
{
    float m = (float)LineCount(e) * LINE_H - ViewHeight(e);
    return m > 0 ? m : 0;
}

static void KeepCursorVisible(Editor *e)
{
    int line, col;
    CursorPos(e, &line, &col);
    float top = (float)line * LINE_H;
    if (top < e->scrollY) e->scrollY = top;
    if (top + LINE_H > e->scrollY + ViewHeight(e)) e->scrollY = top + LINE_H - ViewHeight(e);
    if (e->scrollY < 0) e->scrollY = 0;
}

static float CharWidth(void)
{
    return (float)UiMeasureMono("MMMMMMMMMM", FONT_SIZE) / 10.0f;
}

void EditorInit(Editor *e, Rectangle rect)
{
    memset(e, 0, sizeof(*e));
    e->rect = rect;
    e->desiredCol = -1;
}

void EditorSetText(Editor *e, const char *text)
{
    e->len = 0;
    e->text[0] = '\0';
    for (const char *p = text; *p && e->len < EDITOR_MAX - 1; p++)
    {
        if (*p == '\r') continue;
        e->text[e->len++] = (*p == '\t') ? ' ' : *p;
    }
    e->text[e->len] = '\0';
    e->cursor = 0;
    e->desiredCol = -1;
    e->scrollY = 0;
}

static bool Pressed(int key)
{
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------
static void CursorFromMouse(Editor *e, Vector2 mouse)
{
    int line = (int)((mouse.y - e->rect.y - PAD + e->scrollY) / LINE_H);
    int lines = LineCount(e);
    if (line < 0) line = 0;
    if (line >= lines) line = lines - 1;

    int col = (int)((mouse.x - e->rect.x - GUTTER - PAD + CharWidth() / 2.0f) / CharWidth());
    int length = LineLength(e, line);
    if (col < 0) col = 0;
    if (col > length) col = length;

    e->cursor = LineStart(e, line) + col;
    e->desiredCol = -1;
}

void EditorUpdate(Editor *e, bool active)
{
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, e->rect);

    if (hover)
    {
        e->scrollY -= GetMouseWheelMove() * LINE_H * 3.0f;
        if (e->scrollY < 0) e->scrollY = 0;
        if (e->scrollY > MaxScroll(e)) e->scrollY = MaxScroll(e);

        if (active && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) CursorFromMouse(e, mouse);
    }

    if (!active) return;

    bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) ||
                IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
    bool moved = false;

    // Typing
    int c;
    while ((c = GetCharPressed()) > 0)
    {
        if (c >= 32 && c <= 126 && !ctrl)
        {
            char ch = (char)c;
            Insert(e, &ch, 1);
            e->desiredCol = -1;
            moved = true;
        }
    }

    // Enter keeps the indentation, and adds one level after an opening brace
    if (!ctrl && Pressed(KEY_ENTER))
    {
        int line, col;
        CursorPos(e, &line, &col);
        int start = LineStart(e, line);

        int spaces = 0;
        while (spaces < col && e->text[start + spaces] == ' ') spaces++;
        bool afterBrace = e->cursor > 0 && e->text[e->cursor - 1] == '{';

        char buf[80];
        int n = 0;
        buf[n++] = '\n';
        for (int i = 0; i < spaces + (afterBrace ? INDENT : 0) && n < (int)sizeof(buf) - 1; i++) buf[n++] = ' ';
        Insert(e, buf, n);
        e->desiredCol = -1;
        moved = true;
    }

    if (Pressed(KEY_TAB))
    {
        Insert(e, "    ", INDENT);
        e->desiredCol = -1;
        moved = true;
    }

    if (Pressed(KEY_BACKSPACE) && e->cursor > 0)
    {
        int line, col;
        CursorPos(e, &line, &col);
        int start = LineStart(e, line);

        bool onlySpaces = true;
        for (int i = 0; i < col; i++)
        {
            if (e->text[start + i] != ' ') onlySpaces = false;
        }

        int count = (onlySpaces && col >= INDENT && col % INDENT == 0) ? INDENT : 1;   // delete a whole indent
        DeleteAt(e, e->cursor - count, count);
        e->cursor -= count;
        e->desiredCol = -1;
        moved = true;
    }

    if (Pressed(KEY_DELETE) && e->cursor < e->len)
    {
        DeleteAt(e, e->cursor, 1);
        e->desiredCol = -1;
        moved = true;
    }

    // Moving around
    if (Pressed(KEY_LEFT) && e->cursor > 0)       { e->cursor--; e->desiredCol = -1; moved = true; }
    if (Pressed(KEY_RIGHT) && e->cursor < e->len) { e->cursor++; e->desiredCol = -1; moved = true; }

    if (Pressed(KEY_HOME) || Pressed(KEY_END))
    {
        int line, col;
        CursorPos(e, &line, &col);
        e->cursor = LineStart(e, line) + (IsKeyDown(KEY_HOME) ? 0 : LineLength(e, line));
        e->desiredCol = -1;
        moved = true;
    }

    if (Pressed(KEY_UP) || Pressed(KEY_DOWN))
    {
        int line, col;
        CursorPos(e, &line, &col);
        if (e->desiredCol < 0) e->desiredCol = col;

        int target = line + (IsKeyDown(KEY_UP) ? -1 : 1);
        if (target >= 0 && target < LineCount(e))
        {
            int length = LineLength(e, target);
            e->cursor = LineStart(e, target) + (e->desiredCol < length ? e->desiredCol : length);
        }
        moved = true;
    }

    // Paste
    if (ctrl && IsKeyPressed(KEY_V))
    {
        const char *clip = GetClipboardText();
        for (const char *p = clip; clip != NULL && *p; p++)
        {
            if (*p == '\r') continue;
            if (*p == '\t') Insert(e, "    ", INDENT);
            else if (*p == '\n' || (*p >= 32 && *p <= 126)) Insert(e, p, 1);
        }
        e->desiredCol = -1;
        moved = true;
    }

    if (moved) KeepCursorVisible(e);
}

// ---------------------------------------------------------------------------
// Drawing with simple syntax colours
// ---------------------------------------------------------------------------
#define COL_TEXT     ((Color){ 214, 226, 255, 255 })
#define COL_KEYWORD  ((Color){ 199, 146, 234, 255 })
#define COL_TYPE     ((Color){ 96, 165, 255, 255 })
#define COL_FUNC     ((Color){ 130, 220, 255, 255 })
#define COL_NUMBER   ((Color){ 255, 211, 0, 255 })
#define COL_STRING   ((Color){ 126, 231, 135, 255 })
#define COL_COMMENT  ((Color){ 105, 118, 155, 255 })
#define COL_PREPROC  ((Color){ 255, 160, 120, 255 })

static const char *KEYWORDS[] = {
    "if", "else", "for", "while", "do", "return", "break", "continue", "switch", "case", "default",
    "struct", "class", "public", "private", "protected", "new", "delete", "const", "static", "template",
    "typename", "using", "namespace", "nullptr", "true", "false", "this", "sizeof", NULL
};
static const char *TYPES[] = {
    "int", "char", "bool", "double", "float", "long", "short", "unsigned", "void", "string", "vector",
    "map", "list", "size_t", "auto", NULL
};

static bool InList(const char **list, const char *word)
{
    for (int i = 0; list[i] != NULL; i++)
    {
        if (strcmp(list[i], word) == 0) return true;
    }
    return false;
}

static void DrawToken(const char *line, int from, int to, Color color, float x0, float charW, int y)
{
    char tmp[256];
    int n = to - from;
    if (n <= 0 || n >= (int)sizeof(tmp)) return;
    memcpy(tmp, line + from, (size_t)n);
    tmp[n] = '\0';
    UiDrawMono(tmp, (int)(x0 + from * charW), y, FONT_SIZE, color);
}

static void DrawHighlightedLine(const char *line, int n, float x0, float charW, int y)
{
    int first = 0;
    while (first < n && line[first] == ' ') first++;

    if (first < n && line[first] == '#')
    {
        DrawToken(line, first, n, COL_PREPROC, x0, charW, y);
        return;
    }

    int i = first;
    while (i < n)
    {
        char ch = line[i];

        if (ch == ' ')
        {
            i++;
        }
        else if (ch == '/' && i + 1 < n && line[i + 1] == '/')
        {
            DrawToken(line, i, n, COL_COMMENT, x0, charW, y);
            break;
        }
        else if (ch == '"' || ch == '\'')
        {
            int j = i + 1;
            while (j < n && line[j] != ch)
            {
                if (line[j] == '\\' && j + 1 < n) j++;
                j++;
            }
            if (j < n) j++;
            DrawToken(line, i, j, COL_STRING, x0, charW, y);
            i = j;
        }
        else if (isdigit((unsigned char)ch))
        {
            int j = i;
            while (j < n && (isalnum((unsigned char)line[j]) || line[j] == '.')) j++;
            DrawToken(line, i, j, COL_NUMBER, x0, charW, y);
            i = j;
        }
        else if (isalpha((unsigned char)ch) || ch == '_')
        {
            int j = i;
            while (j < n && (isalnum((unsigned char)line[j]) || line[j] == '_')) j++;

            char word[64];
            int wl = j - i < 63 ? j - i : 63;
            memcpy(word, line + i, (size_t)wl);
            word[wl] = '\0';

            int k = j;
            while (k < n && line[k] == ' ') k++;

            Color color = COL_TEXT;
            if (InList(KEYWORDS, word))      color = COL_KEYWORD;
            else if (InList(TYPES, word))    color = COL_TYPE;
            else if (k < n && line[k] == '(') color = COL_FUNC;

            DrawToken(line, i, j, color, x0, charW, y);
            i = j;
        }
        else
        {
            int j = i;
            while (j < n && !isalnum((unsigned char)line[j]) && line[j] != ' ' && line[j] != '_' &&
                   line[j] != '"' && line[j] != '\'' && !(line[j] == '/' && j + 1 < n && line[j + 1] == '/')) j++;
            if (j == i) j = i + 1;
            DrawToken(line, i, j, COL_TEXT, x0, charW, y);
            i = j;
        }
    }
}

void EditorDraw(const Editor *e, bool active)
{
    Rectangle r = e->rect;
    DrawRectangleRounded(r, 0.02f, 6, (Color){ 10, 12, 24, 255 });
    DrawRectangleRoundedLinesEx(r, 0.02f, 6, 2.0f, active ? COL_ACCENT : Fade(COL_MUTED, 0.4f));

    BeginScissorMode((int)r.x + 2, (int)r.y + 2, (int)r.width - 4, (int)r.height - 4);

    DrawRectangle((int)r.x + 2, (int)r.y + 2, GUTTER - 2, (int)r.height - 4, (Color){ 15, 18, 36, 255 });

    float charW = CharWidth();
    float textX = r.x + GUTTER + PAD;
    int cursorLine, cursorCol;
    CursorPos(e, &cursorLine, &cursorCol);

    int line = (int)(e->scrollY / LINE_H);
    float y = r.y + PAD - fmodf(e->scrollY, (float)LINE_H);
    int idx = LineStart(e, line);
    int total = LineCount(e);

    while (line < total && y < r.y + r.height)
    {
        int len = 0;
        while (idx + len < e->len && e->text[idx + len] != '\n') len++;

        if (line == cursorLine)
        {
            DrawRectangle((int)r.x + GUTTER, (int)y, (int)r.width - GUTTER - 2, LINE_H, (Color){ 255, 255, 255, 10 });
        }

        char num[16];
        snprintf(num, sizeof(num), "%d", line + 1);
        UiDrawMono(num, (int)(r.x + GUTTER - 14 - UiMeasureMono(num, 16)), (int)y + 3, 16,
                   line == cursorLine ? COL_TEXT : COL_COMMENT);

        char buf[256];
        int copy = len < (int)sizeof(buf) - 1 ? len : (int)sizeof(buf) - 1;
        memcpy(buf, e->text + idx, (size_t)copy);
        buf[copy] = '\0';
        DrawHighlightedLine(buf, copy, textX, charW, (int)y + 3);

        if (active && line == cursorLine && ((int)(GetTime() * 2.0) % 2 == 0))
        {
            DrawRectangle((int)(textX + cursorCol * charW), (int)y + 3, 2, LINE_H - 6, COL_ACCENT);
        }

        idx += len + 1;
        line++;
        y += LINE_H;
    }

    EndScissorMode();
}