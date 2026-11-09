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

typedef struct {
    char username[FIELD_LEN];
    char fullName[FIELD_LEN];
    char email[FIELD_LEN];
    char course[FIELD_LEN];
    int  experience;       // Experience enum value
    int  xp;
    int  levelsCompleted;  // levels finished, in order
    char created[DATE_LEN];   // YYYY-MM-DD
    char lastLogin[DATE_LEN]; // YYYY-MM-DD
} Account;

typedef struct {
    Account items[MAX_ACCOUNTS];
    int count;
} AccountStore;

const char *ExperienceName(int experience);

void AccountsInit(AccountStore *store);

// A missing file is not an error: the store is simply left empty and true is returned.
bool AccountsLoad(AccountStore *store, const char *path);
bool AccountsSave(const AccountStore *store, const char *path);

// Case-insensitive username lookup. Returns the index or -1.
int AccountsFind(const AccountStore *store, const char *username);

// Returns NULL when the account is valid, otherwise a message for the user.
const char *AccountValidate(const AccountStore *store, const Account *account);

// Returns the new index, or -1 if the store is full.
int AccountsAdd(AccountStore *store, const Account *account);

void AccountTodayString(char out[DATE_LEN]);

#endif