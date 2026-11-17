#include "focus_audio.h"

#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define FOCUS_RATE 22050
#define FOCUS_BUFFER 4096

static AudioStream stream;
static bool initialized;
static short buffer[FOCUS_BUFFER];
static unsigned int seed = 2463534242u;
static double phase;

static unsigned int RandomNext(void)
{
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

void FocusAudioInit(void)
{
    if (initialized) return;
    InitAudioDevice();
    if (!IsAudioDeviceReady()) return;
    stream = LoadAudioStream(FOCUS_RATE, 16, 1);
    if (stream.buffer != NULL)
    {
        initialized = true;
        PlayAudioStream(stream);
        SetAudioStreamVolume(stream, 0.22f);
    }
}

void FocusAudioSetEnabled(bool enabled)
{
    if (!initialized) return;
    if (enabled) ResumeAudioStream(stream);
    else PauseAudioStream(stream);
}

void FocusAudioUpdate(bool enabled)
{
    if (!initialized || !enabled) return;
    if (!IsAudioStreamProcessed(stream)) return;

    // Soft chord bed, vinyl-like noise and a gentle sine pulse. No external
    // downloaded track is required, and it can be muted instantly.
    static const double notes[] = { 220.0, 261.6256, 329.6276, 392.0 };
    for (int i = 0; i < FOCUS_BUFFER; i++)
    {
        double beat = fmod(phase * 2.0, 1.0);
        int chord = ((int)(phase / 4.0) & 3);
        double pad =
            sin(2.0 * PI * notes[chord] * 2.0 * phase) * 0.12 +
            sin(2.0 * PI * notes[(chord + 2) % 4] * 2.0 * phase) * 0.07 +
            sin(2.0 * PI * 55.0 * phase) * 0.10;
        double hiss = ((double)(RandomNext() & 1023) / 1023.0 - 0.5) * 0.025;
        double softBeat = beat < 0.08 ? (0.08 - beat) * 0.25 : 0.0;
        buffer[i] = (short)((pad + hiss + softBeat) * 32767.0);
        phase += 1.0 / FOCUS_RATE;
    }
    UpdateAudioStream(stream, buffer, FOCUS_BUFFER);
}

void FocusAudioShutdown(void)
{
    if (!initialized) return;
    UnloadAudioStream(stream);
    initialized = false;
    CloseAudioDevice();
}
