#ifndef THEME_H
#define THEME_H

#include "raylib.h"

#define SCREEN_W 1280
#define SCREEN_H 720

typedef enum { THEME_DARK = 0, THEME_LIGHT = 1 } AppTheme;

AppTheme ThemeGet(void);
void ThemeSet(AppTheme theme);
void ThemeToggle(void);
bool ThemeLoad(void);
bool ThemeSave(void);
void ThemeSetColorBlind(bool enabled);

Color ThemeBg(void);
Color ThemePanel(void);
Color ThemeAccent(void);
Color ThemeCyan(void);
Color ThemeMuted(void);
Color ThemeDone(void);
Color ThemeLocked(void);
Color ThemeError(void);

#define COL_BG      ThemeBg()
#define COL_PANEL   ThemePanel()
#define COL_ACCENT  ThemeAccent()
#define COL_CYAN    ThemeCyan()
#define COL_MUTED   ThemeMuted()
#define COL_DONE    ThemeDone()
#define COL_LOCKED  ThemeLocked()
#define COL_ERROR   ThemeError()

#endif