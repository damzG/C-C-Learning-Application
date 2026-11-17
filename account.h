// Account storage: raylib-free so it can be unit tested on its own.
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <stdbool.h>

#define MAX_ACCOUNTS 32
#define FIELD_LEN    64
#define DATE_LEN     16
#define ACCOUNTS_FILE "accounts.csv"

typedef enum {
    EXP_BEGINNER = 0,
    EXP_INTERMEDIATE,
    EXP_ADVANCED,
    EXP_COUNT
} Experience;

// Badge bit flags
enum {
    BADGE_STARTER      = 1u << 0,
    BADGE_FIRST_LESSON = 1u << 1,
    BADGE_STREAK_7     = 1u << 2,
    BADGE_POINTER      = 1u << 3,
    BADGE_PLAYGROUND   = 1u << 4,
    BADGE_EULER        = 1u << 5,
    BADGE_JOKER        = 1u << 6,
    BADGE_GRADUATE     = 1u << 7,
    BADGE_COMMUNITY    = 1u << 8,
    BADGE_CHESS        = 1u << 9,
    BADGE_MATH         = 1u << 10,
    BADGE_SCRABBLE     = 1u << 11
};

typedef struct {
    char username[FIELD_LEN];
    char fullName[FIELD_LEN];
    char email[FIELD_LEN];
    char course[FIELD_LEN];
    int  experience;
    int  xp;
    int  levelsCompleted;
    char created[DATE_LEN];
    char lastLogin[DATE_LEN];

    // Progression extras (optional CSV columns; default 0 on old files)
    int  streakDays;
    char lastActivity[DATE_LEN];
    unsigned badges;
    int  pointerWins;
    int  pointerLosses;
    int  pointerBest;
    int  questFlags;
    int  playgroundDone;
    char jokerDate[DATE_LEN];
} Account;

typedef struct {
    Account items[MAX_ACCOUNTS];
    int count;
} AccountStore;

const char *ExperienceName(int experience);
const char *BadgeName(unsigned bit);
int BadgeCount(void);
unsigned BadgeBitAt(int index);

void AccountsInit(AccountStore *store);
bool AccountsLoad(AccountStore *store, const char *path);
bool AccountsSave(const AccountStore *store, const char *path);

int AccountsFind(const AccountStore *store, const char *username);
const char *AccountValidate(const AccountStore *store, const Account *account);
int AccountsAdd(AccountStore *store, const Account *account);

bool AccountCompleteLevel(Account *account, int levelIndex, int xp);
void AccountTodayString(char out[DATE_LEN]);

// Streak / badges / quests
void AccountTouchActivity(Account *account);
bool AccountHasBadge(const Account *account, unsigned bit);
bool AccountAwardBadge(Account *account, unsigned bit); // true if newly awarded
bool AccountQuestDone(const Account *account, int questId);
bool AccountCompleteQuest(Account *account, int questId, int xpReward);

void AccountInitials(const Account *account, char out[8]);

#endif
