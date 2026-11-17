#include "theme.h"

#include <stdio.h>

#define THEME_FILE "theme.txt"

static AppTheme currentTheme = THEME_DARK;

AppTheme ThemeGet(void) { return currentTheme; }
void ThemeSet(AppTheme theme)
{
    currentTheme = (theme == THEME_LIGHT) ? THEME_LIGHT : THEME_DARK;
}
void ThemeToggle(void)
{
    currentTheme = (currentTheme == THEME_DARK) ? THEME_LIGHT : THEME_DARK;
    ThemeSave();
}
bool ThemeLoad(void)
{
    FILE *f = fopen(THEME_FILE, "r");
    if (!f) return false;
    char value[32] = {0};
    if (fscanf(f, "%31s", value) == 1)
        currentTheme = (value[0] == 'l' || value[0] == 'L') ? THEME_LIGHT : THEME_DARK;
    fclose(f);
    return true;
}
bool ThemeSave(void)
{
    FILE *f = fopen(THEME_FILE, "w");
    if (!f) return false;
    fprintf(f, "%s\n", currentTheme == THEME_LIGHT ? "light" : "dark");
    fclose(f);
    return true;
}

Color ThemeBg(void)
{
    return currentTheme == THEME_LIGHT ? (Color){238,242,248,255} : (Color){16,22,47,255};
}
Color ThemePanel(void)
{
    return currentTheme == THEME_LIGHT ? (Color){255,255,255,255} : (Color){28,35,68,255};
}
Color ThemeAccent(void)
{
    return currentTheme == THEME_LIGHT ? (Color){205,150,0,255} : (Color){255,211,0,255};
}
Color ThemeCyan(void)
{
    return currentTheme == THEME_LIGHT ? (Color){45,112,190,255} : (Color){96,165,255,255};
}
Color ThemeMuted(void)
{
    return currentTheme == THEME_LIGHT ? (Color){78,88,112,255} : (Color){160,168,200,255};
}
Color ThemeDone(void)
{
    return currentTheme == THEME_LIGHT ? (Color){20,138,92,255} : (Color){40,214,140,255};
}
Color ThemeLocked(void)
{
    return currentTheme == THEME_LIGHT ? (Color){196,202,214,255} : (Color){58,66,104,255};
}
Color ThemeError(void)
{
    return currentTheme == THEME_LIGHT ? (Color){190,55,68,255} : (Color){255,99,110,255};
}
