#include "account.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CSV_MIN 9
#define CSV_MAX 18
#define LINE_LEN 1024

static const char *EXPERIENCE_NAMES[EXP_COUNT] = { "Beginner", "Intermediate", "Advanced" };

static const unsigned BADGE_BITS[] = {
    BADGE_STARTER, BADGE_FIRST_LESSON, BADGE_STREAK_7, BADGE_POINTER,
    BADGE_PLAYGROUND, BADGE_EULER, BADGE_JOKER, BADGE_GRADUATE,
    BADGE_COMMUNITY, BADGE_CHESS, BADGE_MATH, BADGE_SCRABBLE
};
static const char *BADGE_NAMES[] = {
    "Park Starter", "First Lesson", "7-Day Streak", "Pointer Champ",
    "PlayGround Builder", "Euler Explorer", "Joker Daily", "C^2 Graduate",
    "Community Helper", "Chess Break", "Math Wizard", "Scrabble Star"
};

const char *ExperienceName(int experience)
{
    if (experience < 0 || experience >= EXP_COUNT) return EXPERIENCE_NAMES[0];
    return EXPERIENCE_NAMES[experience];
}

int BadgeCount(void) { return (int)(sizeof(BADGE_BITS) / sizeof(BADGE_BITS[0])); }
unsigned BadgeBitAt(int index)
{
    if (index < 0 || index >= BadgeCount()) return 0;
    return BADGE_BITS[index];
}
const char *BadgeName(unsigned bit)
{
    for (int i = 0; i < BadgeCount(); i++)
        if (BADGE_BITS[i] == bit) return BADGE_NAMES[i];
    return "Badge";
}

static void CopyStr(char *dst, size_t size, const char *src)
{
    size_t i = 0;
    while (i + 1 < size && src[i] != '\0') { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

void AccountsInit(AccountStore *store)
{
    memset(store, 0, sizeof(*store));
}

void AccountTodayString(char out[DATE_LEN])
{
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    if (local == NULL || strftime(out, DATE_LEN, "%Y-%m-%d", local) == 0)
        CopyStr(out, DATE_LEN, "unknown");
}

void AccountInitials(const Account *account, char out[8])
{
    out[0] = '\0';
    const char *p = account->fullName;
    int n = 0;
    while (*p && n < 3)
    {
        while (*p == ' ') p++;
        if (!*p) break;
        out[n++] = (char)toupper((unsigned char)*p);
        while (*p && *p != ' ') p++;
    }
    out[n] = '\0';
    if (n == 0)
    {
        out[0] = (char)toupper((unsigned char)account->username[0]);
        out[1] = '\0';
    }
}

static void WriteTextField(FILE *f, const char *s)
{
    fputc('"', f);
    for (; *s; s++)
    {
        if (*s == '"') fputc('"', f);
        fputc(*s, f);
    }
    fputc('"', f);
}

static int ParseCsvLine(const char *line, char fields[][FIELD_LEN], int maxFields)
{
    int n = 0;
    const char *p = line;
    while (n < maxFields)
    {
        int len = 0;
        fields[n][0] = '\0';
        if (*p == '"')
        {
            p++;
            while (*p)
            {
                if (*p == '"')
                {
                    if (p[1] == '"') { if (len < FIELD_LEN - 1) fields[n][len++] = '"'; p += 2; }
                    else { p++; break; }
                }
                else { if (len < FIELD_LEN - 1) fields[n][len++] = *p; p++; }
            }
        }
        else
        {
            while (*p && *p != ',' && *p != '\n' && *p != '\r')
            {
                if (len < FIELD_LEN - 1) fields[n][len++] = *p;
                p++;
            }
        }
        fields[n][len] = '\0';
        n++;
        if (*p == ',') p++;
        else break;
    }
    return n;
}

static int ClampInt(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

bool AccountsLoad(AccountStore *store, const char *path)
{
    AccountsInit(store);
    FILE *f = fopen(path, "r");
    if (f == NULL) return true;

    char line[LINE_LEN];
    bool firstLine = true;
    while (fgets(line, sizeof(line), f) != NULL)
    {
        if (firstLine)
        {
            firstLine = false;
            if (strncmp(line, "username", 8) == 0) continue;
        }
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;
        if (store->count >= MAX_ACCOUNTS) break;

        char fields[CSV_MAX][FIELD_LEN];
        int n = ParseCsvLine(line, fields, CSV_MAX);
        if (n < CSV_MIN) continue;

        Account *a = &store->items[store->count];
        memset(a, 0, sizeof(*a));
        CopyStr(a->username, FIELD_LEN, fields[0]);
        CopyStr(a->fullName, FIELD_LEN, fields[1]);
        CopyStr(a->email, FIELD_LEN, fields[2]);
        CopyStr(a->course, FIELD_LEN, fields[3]);
        a->experience = ClampInt(atoi(fields[4]), 0, EXP_COUNT - 1);
        a->xp = ClampInt(atoi(fields[5]), 0, 1000000);
        a->levelsCompleted = ClampInt(atoi(fields[6]), 0, 1000);
        CopyStr(a->created, DATE_LEN, fields[7]);
        CopyStr(a->lastLogin, DATE_LEN, fields[8]);
        if (n > 9) a->streakDays = ClampInt(atoi(fields[9]), 0, 100000);
        if (n > 10) CopyStr(a->lastActivity, DATE_LEN, fields[10]);
        if (n > 11) a->badges = (unsigned)strtoul(fields[11], NULL, 10);
        if (n > 12) a->pointerWins = ClampInt(atoi(fields[12]), 0, 1000000);
        if (n > 13) a->pointerLosses = ClampInt(atoi(fields[13]), 0, 1000000);
        if (n > 14) a->pointerBest = ClampInt(atoi(fields[14]), 0, 1000000);
        if (n > 15) a->questFlags = atoi(fields[15]);
        if (n > 16) a->playgroundDone = ClampInt(atoi(fields[16]), 0, 100000);
        if (n > 17) CopyStr(a->jokerDate, DATE_LEN, fields[17]);
        store->count++;
    }
    fclose(f);
    return true;
}

bool AccountsSave(const AccountStore *store, const char *path)
{
    FILE *f = fopen(path, "w");
    if (f == NULL) return false;

    fprintf(f, "username,full_name,email,course,experience,xp,levels_completed,created,last_login,"
               "streak,last_activity,badges,pointer_wins,pointer_losses,pointer_best,quests,playground,joker_date\n");

    for (int i = 0; i < store->count; i++)
    {
        const Account *a = &store->items[i];
        WriteTextField(f, a->username);  fputc(',', f);
        WriteTextField(f, a->fullName);  fputc(',', f);
        WriteTextField(f, a->email);     fputc(',', f);
        WriteTextField(f, a->course);    fputc(',', f);
        fprintf(f, "%d,%d,%d,", a->experience, a->xp, a->levelsCompleted);
        WriteTextField(f, a->created);   fputc(',', f);
        WriteTextField(f, a->lastLogin); fputc(',', f);
        fprintf(f, "%d,", a->streakDays);
        WriteTextField(f, a->lastActivity); fputc(',', f);
        fprintf(f, "%u,%d,%d,%d,%d,%d,", a->badges, a->pointerWins, a->pointerLosses,
                a->pointerBest, a->questFlags, a->playgroundDone);
        WriteTextField(f, a->jokerDate);
        fputc('\n', f);
    }
    return fclose(f) == 0;
}

static bool EqualsIgnoreCase(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return false;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

int AccountsFind(const AccountStore *store, const char *username)
{
    for (int i = 0; i < store->count; i++)
        if (EqualsIgnoreCase(store->items[i].username, username)) return i;
    return -1;
}

static bool HasText(const char *s)
{
    for (; *s; s++) if (!isspace((unsigned char)*s)) return true;
    return false;
}

static bool LooksLikeEmail(const char *email)
{
    const char *at = strchr(email, '@');
    if (at == NULL || at == email) return false;
    if (strchr(email, ' ') != NULL) return false;
    const char *dot = strchr(at + 1, '.');
    if (dot == NULL || dot == at + 1) return false;
    return dot[1] != '\0';
}

const char *AccountValidate(const AccountStore *store, const Account *a)
{
    if (!HasText(a->fullName)) return "Please enter your full name.";
    size_t userLen = strlen(a->username);
    if (userLen < 3 || userLen > 20) return "Username must be 3 to 20 characters.";
    for (size_t i = 0; i < userLen; i++)
    {
        unsigned char c = (unsigned char)a->username[i];
        if (!isalnum(c) && c != '_') return "Username can only use letters, numbers and _.";
    }
    if (!LooksLikeEmail(a->email)) return "Please enter a valid email address.";
    if (!HasText(a->course)) return "Please enter your course or school.";
    if (AccountsFind(store, a->username) >= 0) return "That username is already taken.";
    if (store->count >= MAX_ACCOUNTS) return "Profile limit reached.";
    return NULL;
}

bool AccountCompleteLevel(Account *account, int levelIndex, int xp)
{
    if (levelIndex != account->levelsCompleted) return false;
    account->levelsCompleted++;
    account->xp += xp;
    AccountTouchActivity(account);
    AccountAwardBadge(account, BADGE_FIRST_LESSON);
    return true;
}

bool AccountHasBadge(const Account *account, unsigned bit)
{
    return (account->badges & bit) != 0;
}

bool AccountAwardBadge(Account *account, unsigned bit)
{
    if (account->badges & bit) return false;
    account->badges |= bit;
    return true;
}

bool AccountQuestDone(const Account *account, int questId)
{
    if (questId < 0 || questId >= 31) return false;
    return (account->questFlags & (1 << questId)) != 0;
}

bool AccountCompleteQuest(Account *account, int questId, int xpReward)
{
    if (questId < 0 || questId >= 31) return false;
    if (account->questFlags & (1 << questId)) return false;
    account->questFlags |= (1 << questId);
    account->xp += xpReward;
    AccountTouchActivity(account);
    return true;
}

static int DayNumber(const char *ymd)
{
    int y = 0, m = 0, d = 0;
    if (sscanf(ymd, "%d-%d-%d", &y, &m, &d) != 3) return 0;
    return y * 372 + m * 31 + d;
}

void AccountTouchActivity(Account *account)
{
    char today[DATE_LEN];
    AccountTodayString(today);
    if (strcmp(account->lastActivity, today) == 0) return;

    if (account->lastActivity[0] == '\0')
    {
        account->streakDays = 1;
    }
    else
    {
        int diff = DayNumber(today) - DayNumber(account->lastActivity);
        if (diff == 1) account->streakDays += 1;
        else account->streakDays = 1;
    }
    CopyStr(account->lastActivity, DATE_LEN, today);
    if (account->streakDays >= 7) AccountAwardBadge(account, BADGE_STREAK_7);
}

int AccountsAdd(AccountStore *store, const Account *account)
{
    if (store->count >= MAX_ACCOUNTS) return -1;
    store->items[store->count] = *account;
    AccountAwardBadge(&store->items[store->count], BADGE_STARTER);
    return store->count++;
}
