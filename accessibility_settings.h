#ifndef ACCESSIBILITY_SETTINGS_H
#define ACCESSIBILITY_SETTINGS_H

#include <stdbool.h>

typedef enum { TEXT_SMALL = 0, TEXT_MEDIUM = 1, TEXT_LARGE = 2 } TextSize;

typedef struct {
    TextSize textSize;
    bool reducedMotion;
    bool colorBlindPalette;
    bool keyboardOnly;
    float musicVolume;
} AccessibilitySettings;

void AccessibilitySettingsInit(AccessibilitySettings *settings);
bool AccessibilitySettingsLoad(AccessibilitySettings *settings);
bool AccessibilitySettingsSave(const AccessibilitySettings *settings);
float AccessibilityTextScale(const AccessibilitySettings *settings);

#endif
