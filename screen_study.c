// Study Tools: cheatsheets and flashcards.
#include "screens.h"
#include "theme.h"
#include "ui.h"
#include "study_content.h"

#include <stdio.h>
#include <string.h>

typedef enum { STUDY_HUB, STUDY_SHEET, STUDY_DECKS, STUDY_CARDS } StudyMode;

static StudyMode mode;
static int sheetIndex;
static int deckIndex;
static int cardIndex;
static bool showBack;
static int knownCount;
static float scroll;

static bool BackBtn(void)
{
    return UiButton((Rectangle){ 30, 30, 120, 44 }, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE);
}

void ScreenStudyEnter(AppState *app)
{
    (void)app;
    mode = STUDY_HUB;
    sheetIndex = 0;
    deckIndex = 0;
    cardIndex = 0;
    showBack = false;
    knownCount = 0;
    scroll = 0.0f;
}

static void RunHub(AppState *app)
{
    UiDrawCenteredText("Study Tools", SCREEN_W / 2, 48, 44, RAYWHITE);
    UiDrawCenteredText("Cheatsheets for quick reference · Flashcards for recall", SCREEN_W / 2, 100, 20, COL_MUTED);

    Rectangle left = { 120, 160, 480, 420 };
    Rectangle right = { 680, 160, 480, 420 };
    Vector2 mouse = GetMousePosition();
    bool hL = CheckCollisionPointRec(mouse, left);
    bool hR = CheckCollisionPointRec(mouse, right);

    DrawRectangleRounded(left, 0.08f, 8, COL_PANEL);
    DrawRectangleRoundedLinesEx(left, 0.08f, 8, 3.0f, hL ? COL_ACCENT : Fade(COL_MUTED, 0.4f));
    DrawCircle((int)(left.x + left.width / 2), (int)left.y + 90, 40, COL_ACCENT);
    UiDrawCenteredText("Cheatsheets", (int)(left.x + left.width / 2), (int)left.y + 160, 32, RAYWHITE);
    UiDrawWrappedText("C, C++, OOP, Big-O complexities and recurring coding patterns in one place.",
                      (Rectangle){ left.x + 36, left.y + 220, left.width - 72, 120 }, 20, COL_MUTED);
    char sc[32];
    snprintf(sc, sizeof(sc), "%d sheets", CheatSheetCount());
    UiDrawCenteredText(sc, (int)(left.x + left.width / 2), (int)left.y + 360, 20, COL_ACCENT);

    DrawRectangleRounded(right, 0.08f, 8, COL_PANEL);
    DrawRectangleRoundedLinesEx(right, 0.08f, 8, 3.0f, hR ? COL_CYAN : Fade(COL_MUTED, 0.4f));
    DrawCircle((int)(right.x + right.width / 2), (int)right.y + 90, 40, COL_CYAN);
    UiDrawCenteredText("Flashcards", (int)(right.x + right.width / 2), (int)right.y + 160, 32, RAYWHITE);
    UiDrawWrappedText("Flip cards by topic. Mark what you know and drill the rest.",
                      (Rectangle){ right.x + 36, right.y + 220, right.width - 72, 120 }, 20, COL_MUTED);
    char dc[32];
    snprintf(dc, sizeof(dc), "%d decks", FlashDeckCount());
    UiDrawCenteredText(dc, (int)(right.x + right.width / 2), (int)right.y + 360, 20, COL_CYAN);

    if (hL && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { mode = STUDY_SHEET; sheetIndex = 0; scroll = 0; }
    if (hR && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { mode = STUDY_DECKS; }

    if (BackBtn()) AppGoTo(app, SCREEN_MENU);
}

static void RunSheet(AppState *app)
{
    (void)app;
    const CheatSheet *sheet = CheatSheetGet(sheetIndex);
    if (!sheet) { mode = STUDY_HUB; return; }

    UiDrawCenteredText(sheet->title, SCREEN_W / 2, 36, 36, RAYWHITE);
    UiDrawCenteredText(sheet->subtitle, SCREEN_W / 2, 80, 20, COL_MUTED);

    // Category tabs
    float tabW = 200.0f;
    float tabX = (SCREEN_W - CheatSheetCount() * tabW) / 2.0f;
    for (int i = 0; i < CheatSheetCount(); i++)
    {
        const CheatSheet *s = CheatSheetGet(i);
        Rectangle r = { tabX + i * tabW, 110, tabW - 8, 40 };
        bool on = (i == sheetIndex);
        if (UiButton(r, s->title, true, 1.0f))
        {
            sheetIndex = i;
            scroll = 0;
            sheet = CheatSheetGet(sheetIndex);
        }
        if (on) DrawRectangleRoundedLinesEx(r, 0.2f, 6, 2.0f, COL_ACCENT);
    }

    scroll -= GetMouseWheelMove() * 36.0f;
    if (scroll < 0) scroll = 0;

    Rectangle panel = { 100, 170, SCREEN_W - 200, 480 };
    DrawRectangleRounded(panel, 0.06f, 8, COL_PANEL);

    BeginScissorMode((int)panel.x, (int)panel.y, (int)panel.width, (int)panel.height);
    float y = panel.y + 24 - scroll;
    const char *p = sheet->body;
    while (p && *p)
    {
        char line[160];
        int n = 0;
        while (p[n] && p[n] != '\n' && n < (int)sizeof(line) - 1) { line[n] = p[n]; n++; }
        line[n] = '\0';
        UiDrawMono(line, (int)panel.x + 28, (int)y, 20, RAYWHITE);
        y += 28;
        p += n;
        if (*p == '\n') p++;
    }
    EndScissorMode();

    float contentH = y - (panel.y + 24 - scroll) + 40;
    float maxScroll = contentH - panel.height;
    if (maxScroll < 0) maxScroll = 0;
    if (scroll > maxScroll) scroll = maxScroll;

    if (UiButton((Rectangle){ SCREEN_W / 2.0f - 260, 660, 240, 44 }, "Prev sheet", sheetIndex > 0, 1.0f))
    {
        sheetIndex--;
        scroll = 0;
    }
    if (UiButton((Rectangle){ SCREEN_W / 2.0f + 20, 660, 240, 44 }, "Next sheet", sheetIndex + 1 < CheatSheetCount(), 1.0f))
    {
        sheetIndex++;
        scroll = 0;
    }

    if (BackBtn()) mode = STUDY_HUB;
}

static void RunDecks(AppState *app)
{
    (void)app;
    UiDrawCenteredText("Flashcard Decks", SCREEN_W / 2, 48, 40, RAYWHITE);
    UiDrawCenteredText("Pick a deck to start flipping", SCREEN_W / 2, 100, 20, COL_MUTED);

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < FlashDeckCount(); i++)
    {
        const FlashDeck *d = FlashDeckGet(i);
        int col = i % 3;
        int row = i / 3;
        Rectangle r = { 110.0f + col * 360.0f, 150.0f + row * 220.0f, 330, 190 };
        bool h = CheckCollisionPointRec(mouse, r);
        DrawRectangleRounded(r, 0.08f, 8, COL_PANEL);
        DrawRectangleRoundedLinesEx(r, 0.08f, 8, 3.0f, h ? COL_CYAN : Fade(COL_MUTED, 0.4f));
        UiDrawCenteredText(d->title, (int)(r.x + r.width / 2), (int)r.y + 40, 28, RAYWHITE);
        UiDrawWrappedText(d->blurb, (Rectangle){ r.x + 24, r.y + 90, r.width - 48, 50 }, 18, COL_MUTED);
        char n[32];
        snprintf(n, sizeof(n), "%d cards", d->cardCount);
        UiDrawCenteredText(n, (int)(r.x + r.width / 2), (int)r.y + 150, 18, COL_CYAN);
        if (h && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            deckIndex = i;
            cardIndex = 0;
            showBack = false;
            knownCount = 0;
            mode = STUDY_CARDS;
        }
    }

    if (BackBtn()) mode = STUDY_HUB;
}

static void RunCards(AppState *app)
{
    (void)app;
    const FlashDeck *deck = FlashDeckGet(deckIndex);
    if (!deck || deck->cardCount <= 0) { mode = STUDY_DECKS; return; }
    if (cardIndex >= deck->cardCount) cardIndex = deck->cardCount - 1;

    const FlashCard *card = &deck->cards[cardIndex];
    UiDrawCenteredText(deck->title, SCREEN_W / 2, 40, 32, RAYWHITE);

    char progress[64];
    snprintf(progress, sizeof(progress), "Card %d / %d    Marked known: %d",
             cardIndex + 1, deck->cardCount, knownCount);
    UiDrawCenteredText(progress, SCREEN_W / 2, 90, 20, COL_MUTED);

    Rectangle cardR = { 240, 140, 800, 360 };
    Vector2 mouse = GetMousePosition();
    bool h = CheckCollisionPointRec(mouse, cardR);
    DrawRectangleRounded(cardR, 0.06f, 10, COL_PANEL);
    DrawRectangleRoundedLinesEx(cardR, 0.06f, 10, 4.0f, showBack ? COL_DONE : COL_ACCENT);

    UiDrawCenteredText(showBack ? "Answer" : "Question", SCREEN_W / 2, 170, 22, COL_MUTED);
    UiDrawWrappedText(showBack ? card->back : card->front,
                      (Rectangle){ cardR.x + 60, cardR.y + 100, cardR.width - 120, 200 }, 28, RAYWHITE);
    UiDrawCenteredText(h ? "Click to flip" : " ", SCREEN_W / 2, 460, 18, COL_CYAN);

    if (h && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) showBack = !showBack;
    if (IsKeyPressed(KEY_SPACE)) showBack = !showBack;

    if (UiButton((Rectangle){ 200, 540, 200, 52 }, "Prev", cardIndex > 0, 1.0f))
    {
        cardIndex--;
        showBack = false;
    }
    if (UiButton((Rectangle){ 420, 540, 200, 52 }, "I know this", true, 1.0f))
    {
        knownCount++;
        UiToast("Nice!");
        if (cardIndex + 1 < deck->cardCount) { cardIndex++; showBack = false; }
    }
    if (UiButton((Rectangle){ 640, 540, 200, 52 }, "Flip", true, 1.0f)) showBack = !showBack;
    if (UiButton((Rectangle){ 860, 540, 200, 52 }, "Next", cardIndex + 1 < deck->cardCount, 1.0f))
    {
        cardIndex++;
        showBack = false;
    }

    if (BackBtn()) mode = STUDY_DECKS;
}

void ScreenStudyRun(AppState *app, float dt)
{
    (void)dt;
    switch (mode)
    {
    case STUDY_HUB:   RunHub(app);   break;
    case STUDY_SHEET: RunSheet(app); break;
    case STUDY_DECKS: RunDecks(app); break;
    case STUDY_CARDS: RunCards(app); break;
    }
}
