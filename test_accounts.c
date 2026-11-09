// Unit tests for the raylib-free modules. Run with: ctest  (or ./test_accounts)

#include <stdio.h>
#include <string.h>

#include "account.h"
#include "levels.h"

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

static Account MakeAccount(const char *user, const char *name)
{
    Account a;
    memset(&a, 0, sizeof(a));
    snprintf(a.username, FIELD_LEN, "%s", user);
    snprintf(a.fullName, FIELD_LEN, "%s", name);
    snprintf(a.email, FIELD_LEN, "%s@example.com", user);
    snprintf(a.course, FIELD_LEN, "Advanced Programming");
    snprintf(a.created, DATE_LEN, "2026-11-05");
    snprintf(a.lastLogin, DATE_LEN, "2026-11-05");
    return a;
}

int main(void)
{
    const char *path = "test_accounts.csv";
    remove(path);

    // Missing file gives an empty store
    AccountStore store;
    CHECK(AccountsLoad(&store, path));
    CHECK(store.count == 0);

    // Validation
    Account good = MakeAccount("ada_l", "Ada Lovelace");
    CHECK(AccountValidate(&store, &good) == NULL);

    Account bad = good;
    bad.fullName[0] = '\0';
    CHECK(AccountValidate(&store, &bad) != NULL);

    bad = good; snprintf(bad.username, FIELD_LEN, "ab");
    CHECK(AccountValidate(&store, &bad) != NULL);

    bad = good; snprintf(bad.username, FIELD_LEN, "has space");
    CHECK(AccountValidate(&store, &bad) != NULL);

    bad = good; snprintf(bad.email, FIELD_LEN, "not-an-email");
    CHECK(AccountValidate(&store, &bad) != NULL);

    bad = good; snprintf(bad.email, FIELD_LEN, "a@b");
    CHECK(AccountValidate(&store, &bad) != NULL);

    bad = good; bad.course[0] = '\0';
    CHECK(AccountValidate(&store, &bad) != NULL);

    // Add and duplicate detection (case-insensitive)
    CHECK(AccountsAdd(&store, &good) == 0);
    Account dup = MakeAccount("ADA_L", "Someone Else");
    CHECK(AccountValidate(&store, &dup) != NULL);
    CHECK(AccountsFind(&store, "Ada_L") == 0);
    CHECK(AccountsFind(&store, "nobody") == -1);

    // Round trip with commas and quotes in the text
    Account tricky = MakeAccount("grace", "Grace \"Amazing\" Hopper, Jr.");
    snprintf(tricky.course, FIELD_LEN, "Computing, Year 2 \"Hons\"");
    tricky.experience = EXP_ADVANCED;
    tricky.xp = 340;
    tricky.levelsCompleted = 5;
    CHECK(AccountsAdd(&store, &tricky) == 1);
    CHECK(AccountsSave(&store, path));

    AccountStore loaded;
    CHECK(AccountsLoad(&loaded, path));
    CHECK(loaded.count == 2);
    CHECK(strcmp(loaded.items[0].username, "ada_l") == 0);
    CHECK(strcmp(loaded.items[1].fullName, "Grace \"Amazing\" Hopper, Jr.") == 0);
    CHECK(strcmp(loaded.items[1].course, "Computing, Year 2 \"Hons\"") == 0);
    CHECK(loaded.items[1].experience == EXP_ADVANCED);
    CHECK(loaded.items[1].xp == 340);
    CHECK(loaded.items[1].levelsCompleted == 5);
    CHECK(strcmp(loaded.items[1].lastLogin, "2026-11-05") == 0);

    // A damaged row is skipped, not a crash
    FILE *f = fopen(path, "a");
    fputs("broken,row\n", f);
    fclose(f);
    CHECK(AccountsLoad(&loaded, path));
    CHECK(loaded.count == 2);

    // Store limit
    AccountStore full;
    AccountsInit(&full);
    for (int i = 0; i < MAX_ACCOUNTS; i++)
    {
        char user[16];
        snprintf(user, sizeof(user), "user%02d", i);
        Account a = MakeAccount(user, "Test User");
        CHECK(AccountsAdd(&full, &a) == i);
    }
    Account extra = MakeAccount("extra", "Extra User");
    CHECK(AccountsAdd(&full, &extra) == -1);
    CHECK(AccountValidate(&full, &extra) != NULL);

    // Level data
    CHECK(LevelCount() > 0);
    for (int i = 0; i < LevelCount(); i++)
    {
        const Level *l = LevelGet(i);
        CHECK(l != NULL);
        CHECK(l->section >= 0 && l->section < SectionCount());
        CHECK(l->xp > 0);
    }
    CHECK(LevelGet(-1) == NULL);
    CHECK(LevelGet(LevelCount()) == NULL);

    remove(path);

    if (failures == 0) printf("All tests passed.\n");
    return failures == 0 ? 0 : 1;
}