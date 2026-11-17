// Cheatsheets and flashcards: raylib-free study data.
#ifndef STUDY_CONTENT_H
#define STUDY_CONTENT_H

typedef enum {
    SHEET_C = 0,
    SHEET_CPP,
    SHEET_OOP,
    SHEET_COMPLEXITY,
    SHEET_PATTERNS,
    SHEET_CS_FUNDAMENTALS,
    SHEET_SYSTEM_DESIGN,
    SHEET_AI_KNOWLEDGE,
    SHEET_COUNT
} CheatSheetId;

typedef enum {
    DECK_C_BASICS = 0,
    DECK_POINTERS,
    DECK_OOP,
    DECK_DSA,
    DECK_COMPLEXITY,
    DECK_SYSTEM_DESIGN,
    DECK_AI,
    DECK_COUNT
} FlashDeckId;

typedef struct {
    CheatSheetId id;
    const char *title;
    const char *subtitle;
    const char *body;   // \n separated reference lines
} CheatSheet;

typedef struct {
    const char *front;
    const char *back;
} FlashCard;

typedef struct {
    FlashDeckId id;
    const char *title;
    const char *blurb;
    int cardCount;
    const FlashCard *cards;
} FlashDeck;

int CheatSheetCount(void);
const CheatSheet *CheatSheetGet(int index);

int FlashDeckCount(void);
const FlashDeck *FlashDeckGet(int index);

#endif
