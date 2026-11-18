#ifndef FOCUS_AUDIO_H
#define FOCUS_AUDIO_H

#include <stdbool.h>

void FocusAudioInit(void);
void FocusAudioShutdown(void);
void FocusAudioSetEnabled(bool enabled);
void FocusAudioUpdate(bool enabled);
void FocusAudioNextTrack(void);
void FocusAudioSetVolume(float volume);
const char *FocusAudioTrackName(void);

#endif
