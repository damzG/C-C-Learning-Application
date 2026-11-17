#ifndef FOCUS_AUDIO_H
#define FOCUS_AUDIO_H

#include <stdbool.h>

void FocusAudioInit(void);
void FocusAudioShutdown(void);
void FocusAudioSetEnabled(bool enabled);
void FocusAudioUpdate(bool enabled);

#endif
