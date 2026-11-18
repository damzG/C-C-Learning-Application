#include "community_hub.h"
#include "theme.h"
#include "ui.h"
#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[64];
    char course[64];
    int xp;
    int streak;
    bool isCurrentUser;
} LeaderEntry;

static CommunityTab activeTab = HUB_TAB_LEADERBOARD;
static bool walkthroughActive = false;
static int walkthroughStep = 0;

static const char *LeagueName(int xp)
{
    if (xp >= 4001) return "DIAMOND LEAGUE";
    if (xp >= 1501) return "GOLD LEAGUE";
    if (xp >= 501)  return "SILVER LEAGUE";
    return "BRONZE LEAGUE";
}

static Color LeagueColor(int xp)
{
    if (xp >= 4001) return (Color){ 56, 189, 248, 255 };  // Diamond Cyan
    if (xp >= 1501) return (Color){ 250, 204, 21, 255 };  // Gold
    if (xp >= 501)  return (Color){ 203, 213, 225, 255 }; // Silver
    return (Color){ 217, 119, 6, 255 };                   // Bronze
}

static int CompareLeader(const void *a, const void *b)
{
    const LeaderEntry *ea = (const LeaderEntry *)a;
    const LeaderEntry *eb = (const LeaderEntry *)b;
    return eb->xp - ea->xp;
}

void CommunityHubEnter(CommunityTab initialTab)
{
    activeTab = initialTab;
}

bool CommunityHubRun(AppState *app, float dt)
{
    (void)dt;
    if (UiButton((Rectangle){ 30, 26, 120, 48 }, "Back", true, 1.0f) || IsKeyPressed(KEY_ESCAPE))
        return true;

    UiDrawCenteredText("C\xC2\xB2 Community & Info", SCREEN_W / 2, 26, 30, RAYWHITE);

    // Navigation Tabs (Google Material 3 Segmented Tab Bar)
    const char *tabs[3] = { "League Leaderboard", "About C\xC2\xB2", "Contact Us" };
    for (int i = 0; i < 3; i++)
    {
        Rectangle tr = { 260.0f + i * 260.0f, 82.0f, 240.0f, 48.0f };
        bool sel = ((int)activeTab == i);
        DrawRectangleRounded(tr, 0.25f, 8, sel ? COL_ACCENT : COL_PANEL);
        UiDrawCenteredText(tabs[i], (int)(tr.x + tr.width * 0.5f), (int)tr.y + 14, 18, sel ? BLACK : RAYWHITE);
        if (CheckCollisionPointRec(GetMousePosition(), tr) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            activeTab = (CommunityTab)i;
    }

    Rectangle card = { 160, 150, SCREEN_W - 320, 520 };
    DrawRectangleRounded(card, 0.03f, 8, COL_PANEL);
    DrawRectangleRoundedLinesEx(card, 0.03f, 8, 1.5f, Fade(COL_MUTED, 0.35f));

    if (activeTab == HUB_TAB_LEADERBOARD)
    {
        LeaderEntry list[MAX_ACCOUNTS + 6];
        int count = 0;

        // Populate real saved accounts from AccountStore
        Account *cur = app ? AppCurrentAccount(app) : NULL;
        if (app)
        {
            for (int i = 0; i < app->accounts.count; i++)
            {
                const Account *a = &app->accounts.items[i];
                snprintf(list[count].name, sizeof(list[count].name), "%s", a->fullName);
                snprintf(list[count].course, sizeof(list[count].course), "%s", a->course);
                list[count].xp = a->xp;
                list[count].streak = a->streakDays;
                list[count].isCurrentUser = (cur == a);
                count++;
            }
        }

        // Populate benchmark peers across Bronze, Silver, Gold, and Diamond leagues
        const LeaderEntry peers[] = {
            { "Ada Lovelace",   "BSc Computing",    4450, 21, false },
            { "Alan Turing",    "Computer Science", 2350, 14, false },
            { "Grace Hopper",   "Systems Eng",      1120,  8, false },
            { "Dennis Ritchie", "C Systems Track",   420,  4, false }
        };
        for (int i = 0; i < 4 && count < MAX_ACCOUNTS + 6; i++)
            list[count++] = peers[i];

        qsort(list, (size_t)count, sizeof(LeaderEntry), CompareLeader);

        DrawText("Local Profiles & Sample League Entries (Bronze · Silver · Gold · Diamond)", (int)card.x + 28, (int)card.y + 20, 20, RAYWHITE);

        int maxRows = count < 7 ? count : 7;
        for (int i = 0; i < maxRows; i++)
        {
            Rectangle row = { card.x + 24, card.y + 62 + i * 60.0f, card.width - 48, 50 };
            Color lCol = LeagueColor(list[i].xp);
            DrawRectangleRounded(row, 0.2f, 6, list[i].isCurrentUser ? Fade(COL_ACCENT, 0.22f) : (Color){ 22, 30, 46, 255 });
            if (list[i].isCurrentUser)
                DrawRectangleRoundedLinesEx(row, 0.2f, 6, 2.0f, COL_ACCENT);

            char rankStr[16];
            snprintf(rankStr, sizeof(rankStr), "#%d", i + 1);
            DrawText(rankStr, (int)row.x + 16, (int)row.y + 15, 20, lCol);
            DrawText(list[i].name, (int)row.x + 68, (int)row.y + 10, 18, RAYWHITE);
            DrawText(list[i].course, (int)row.x + 68, (int)row.y + 30, 13, COL_MUTED);

            // League Pill
            Rectangle pill = { row.x + row.width - 320, row.y + 11, 165, 28 };
            DrawRectangleRounded(pill, 0.4f, 6, Fade(lCol, 0.2f));
            UiDrawCenteredText(LeagueName(list[i].xp), (int)(pill.x + pill.width * 0.5f), (int)pill.y + 7, 13, lCol);

            char xpStr[32];
            snprintf(xpStr, sizeof(xpStr), "%d XP", list[i].xp);
            DrawText(xpStr, (int)row.x + (int)row.width - 125, (int)row.y + 15, 19, RAYWHITE);
        }
    }
    else if (activeTab == HUB_TAB_ABOUT)
    {
        // C² Emblem Badge
        DrawRectangleRounded((Rectangle){ card.x + 36, card.y + 32, 76, 76 }, 0.22f, 8, COL_ACCENT);
        UiDrawCenteredText("C\xC2\xB2", (int)card.x + 74, (int)card.y + 52, 34, BLACK);

        DrawText("About C\xC2\xB2 Learning Platform", (int)card.x + 130, (int)card.y + 38, 26, RAYWHITE);
        DrawText("Interactive Visual Computer Science & Engineering Studio", (int)card.x + 130, (int)card.y + 72, 16, COL_CYAN);

        const char *aboutBody =
            "C\xC2\xB2 is an interactive visual learning platform for developers and students. "
            "Watch algorithms come to life with step-by-step visualizations covering sorting, "
            "trees, graphs, dynamic programming, and more.\n\n"
            "Core Curriculum Tracks:\n"
            "  •  C / C++ Memory, Pointers & Systems Programming\n"
            "  •  Data Structures & Algorithms (DSA)\n"
            "  •  Distributed System Design & Scalable Architecture\n"
            "  •  Artificial Intelligence & Neural Inference Pipelines\n"
            "  •  Operating Systems, Computer Networking & DBMS";

        UiDrawWrappedText(aboutBody, (Rectangle){ card.x + 36, card.y + 130, card.width - 72, 280 }, 19, RAYWHITE);

        if (UiButton((Rectangle){ card.x + 36, card.y + 430, 260, 48 }, "Launch App Walkthrough", true, 1.0f))
            WalkthroughStart();
    }
    else if (activeTab == HUB_TAB_CONTACT)
    {
        DrawText("Contact the C\xC2\xB2 Development Team", (int)card.x + 36, (int)card.y + 32, 26, RAYWHITE);
        DrawText("South East Technological University (SETU) — Computing & Software Engineering",
                 (int)card.x + 36, (int)card.y + 68, 16, COL_MUTED);

        const char *names[2]   = { "Oyindamola Olaosun", "Owen Nathanael" };
        const char *emails[2]  = { "C00313475@setu.ie",  "C00313648@setu.ie" };
        const char *roles[2]   = { "Co-Creator · UI/UX & Interactive Visualization Systems",
                                   "Co-Creator · Core Engine, Algorithms & 3D Architecture" };

        for (int i = 0; i < 2; i++)
        {
            Rectangle box = { card.x + 36, card.y + 120 + i * 155.0f, card.width - 72, 130 };
            DrawRectangleRounded(box, 0.14f, 8, (Color){ 22, 30, 46, 255 });
            DrawRectangleRoundedLinesEx(box, 0.14f, 8, 1.5f, COL_CYAN);

            DrawText(names[i], (int)box.x + 24, (int)box.y + 18, 22, RAYWHITE);
            DrawText(roles[i], (int)box.x + 24, (int)box.y + 48, 15, COL_MUTED);
            DrawText(emails[i], (int)box.x + 24, (int)box.y + 82, 19, COL_ACCENT);

            if (UiButton((Rectangle){ box.x + box.width - 180, box.y + 41, 156, 48 }, "Copy Email", true, 1.0f))
            {
                SetClipboardText(emails[i]);
                UiToast("Email copied to clipboard!");
            }
        }
    }

    return false;
}

// ===================== Interactive App Walkthrough =====================
void WalkthroughStart(void)
{
    walkthroughActive = true;
    walkthroughStep = 0;
}

bool WalkthroughIsActive(void)
{
    return walkthroughActive;
}

void WalkthroughDrawOverlay(void)
{
    if (!walkthroughActive) return;

    const char *titles[4] = {
        "1. Welcome to C\xC2\xB2 Visual Learning",
        "2. Interactive Explore Diagrams",
        "3. 2D & 3D Puzzle & Chess Arena",
        "4. Leagues, Streaks & Accessibility"
    };
    const char *bodies[4] = {
        "Master C/C++, DSA, System Design, AI, OS, Networking, and DBMS through hands-on visual modules.",
        "Open the Explore tab to step through live architecture flows (CPU pipelines, Transformers, and Video Streaming).",
        "Challenge the Minimax Chess AI in 2D or grounded 3D, and sharpen logic with Sort Sprint and Block Drop 3D.",
        "Earn XP to climb from Bronze to Diamond League, and customize text size, reduced motion, or colorblind palettes in Settings."
    };

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.72f));

    Rectangle modal = { SCREEN_W / 2.0f - 340, SCREEN_H / 2.0f - 170, 680, 340 };
    DrawRectangleRounded(modal, 0.05f, 8, COL_PANEL);
    DrawRectangleRoundedLinesEx(modal, 0.05f, 8, 2.0f, COL_ACCENT);

    char badge[32];
    snprintf(badge, sizeof(badge), "STEP %d OF 4", walkthroughStep + 1);
    DrawText(badge, (int)modal.x + 32, (int)modal.y + 26, 14, COL_CYAN);
    DrawText(titles[walkthroughStep], (int)modal.x + 32, (int)modal.y + 52, 24, RAYWHITE);
    UiDrawWrappedText(bodies[walkthroughStep], (Rectangle){ modal.x + 32, modal.y + 100, modal.width - 64, 130 }, 18, RAYWHITE);

    if (UiButton((Rectangle){ modal.x + 32, modal.y + 262, 120, 48 }, "Skip", true, 1.0f))
        walkthroughActive = false;

    const char *nextLbl = (walkthroughStep < 3) ? "Next ->" : "Finish";
    if (UiButton((Rectangle){ modal.x + modal.width - 172, modal.y + 262, 140, 48 }, nextLbl, true, 1.0f))
    {
        walkthroughStep++;
        if (walkthroughStep >= 4) walkthroughActive = false;
    }
}