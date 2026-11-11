#include "screens.h"
#include "theme.h"
#include "ui.h"
#include "levels.h"
#include "avatar_draw.h"

#include <stdio.h>

#define VISIBLE_CARDS 5
#define CARD_W 720.0f
#define CARD_H 88.0f

static int firstShown;

void ScreenProfilesEnter(AppState *app)
{
    (void)app;
    firstShown = 0;
}

static void SignIn(AppState *app, int index)
{
    Account *acc = &app->accounts.items[index];
    app->current = index;
    AccountTodayString(acc->lastLogin);
    AppSaveAccounts(app);

    char msg[96];
    snprintf(msg, sizeof(msg), "Welcome back, %s!", acc->fullName);
    UiToast(msg);
    AppGoTo(app, SCREEN_MAP);
}

void ScreenProfilesRun(AppState *app, float dt)
{
    (void)dt;
    int total = app->accounts.count;

    UiDrawCenteredText("Choose your profile", SCREEN_W / 2, 50, 44, RAYWHITE);

    int maxFirst = total > VISIBLE_CARDS ? total - VISIBLE_CARDS : 0;
    firstShown -= (int)GetMouseWheelMove();
    if (firstShown < 0) firstShown = 0;
    if (firstShown > maxFirst) firstShown = maxFirst;

    for (int k = 0; k < VISIBLE_CARDS; k++)
    {
        int idx = firstShown + k;
        if (idx >= total) break;

        const Account *a = &app->accounts.items[idx];
        Rectangle card = { (SCREEN_W - CARD_W) / 2.0f, 130.0f + k * 100.0f, CARD_W, CARD_H };
        bool hovered = CheckCollisionPointRec(GetMousePosition(), card);

        DrawRectangleRounded(card, 0.2f, 8, COL_PANEL);
        DrawRectangleRoundedLinesEx(card, 0.2f, 8, 2.0f, hovered ? COL_ACCENT : Fade(COL_MUTED, 0.4f));

        // Generated profile picture
        AvatarDraw(a->username, (int)card.x + 16, (int)card.y + 12, 64, 1.0f);

        DrawText(a->fullName, (int)card.x + 100, (int)card.y + 16, 26, RAYWHITE);

        char detail[160];
        snprintf(detail, sizeof(detail), "@%s  |  %s  |  Level %d of %d",
                 a->username, ExperienceName(a->experience),
                 a->levelsCompleted < LevelCount() ? a->levelsCompleted + 1 : LevelCount(),
                 LevelCount());
        DrawText(detail, (int)card.x + 100, (int)card.y + 50, 18, COL_MUTED);

        char xp[32];
        snprintf(xp, sizeof(xp), "%d XP", a->xp);
        DrawText(xp, (int)(card.x + card.width - MeasureText(xp, 24) - 24), (int)card.y + 32, 24, COL_CYAN);

        if (hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            SignIn(app, idx);
            return;
        }
    }

    if (total > VISIBLE_CARDS)
    {
        UiDrawCenteredText("Scroll to see more profiles", SCREEN_W / 2, 625, 18, COL_MUTED);
    }

    if (UiButton((Rectangle){ SCREEN_W / 2.0f - 260, 650, 240, 52 }, "New Profile", true, 1.0f)) AppGoTo(app, SCREEN_CREATE);
    if (UiButton((Rectangle){ SCREEN_W / 2.0f + 20, 650, 240, 52 }, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE))
    {
        AppGoTo(app, SCREEN_MENU);
    }
}