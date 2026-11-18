#include "accessibility_settings.h"

#include <stdio.h>
#include <string.h>

#define SETTINGS_FILE "settings.txt"

void AccessibilitySettingsInit(AccessibilitySettings *settings)
{
    memset(settings, 0, sizeof(*settings));
    settings->textSize = TEXT_MEDIUM;
    settings->musicVolume = 0.32f;
}
bool AccessibilitySettingsLoad(AccessibilitySettings *settings)
{
    AccessibilitySettingsInit(settings);
    FILE *f = fopen(SETTINGS_FILE, "r");
    if (!f) return false;
    char key[32]; char value[32];
    while (fscanf(f, "%31[^=]=%31s\n", key, value) == 2)
    {
        if (strcmp(key, "text") == 0) settings->textSize = value[0] == '0' ? TEXT_SMALL : (value[0] == '2' ? TEXT_LARGE : TEXT_MEDIUM);
        else if (strcmp(key, "motion") == 0) settings->reducedMotion = value[0] == '1';
        else if (strcmp(key, "colorblind") == 0) settings->colorBlindPalette = value[0] == '1';
        else if (strcmp(key, "keyboard") == 0) settings->keyboardOnly = value[0] == '1';
        else if (strcmp(key, "volume") == 0) sscanf(value, "%f", &settings->musicVolume);
    }
    fclose(f);
    return true;
}
bool AccessibilitySettingsSave(const AccessibilitySettings *settings)
{
    FILE *f = fopen(SETTINGS_FILE, "w");
    if (!f) return false;
    fprintf(f, "text=%d\n", (int)settings->textSize);
    fprintf(f, "motion=%d\n", settings->reducedMotion ? 1 : 0);
    fprintf(f, "colorblind=%d\n", settings->colorBlindPalette ? 1 : 0);
    fprintf(f, "keyboard=%d\n", settings->keyboardOnly ? 1 : 0);
    fprintf(f, "volume=%.2f\n", settings->musicVolume);
    fclose(f);
    return true;
}
float AccessibilityTextScale(const AccessibilitySettings *settings)
{
    if (!settings) return 1.0f;
    return settings->textSize == TEXT_SMALL ? 0.86f :
           settings->textSize == TEXT_LARGE ? 1.18f : 1.0f;
}
