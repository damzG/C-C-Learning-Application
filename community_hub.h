#ifndef COMMUNITY_HUB_H
#define COMMUNITY_HUB_H

#include "app.h"
#include <stdbool.h>

typedef enum {
    HUB_TAB_LEADERBOARD = 0,
    HUB_TAB_ABOUT,
    HUB_TAB_CONTACT
} CommunityTab;

void CommunityHubEnter(CommunityTab initialTab);
// Returns true when Back is pressed
bool CommunityHubRun(AppState *app, float dt);

// Interactive Walkthrough Overlay (can be triggered on first login or from Menu)
void WalkthroughStart(void);
bool WalkthroughIsActive(void);
void WalkthroughDrawOverlay(void);

#endif