#include "focus_audio.h"

#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FOCUS_RATE 22050
#define FOCUS_BUFFER 4096

static AudioStream stream;
static bool initialized;
static short buffer[FOCUS_BUFFER];
static unsigned int seed = 2463534242u;
static double phase;
static Music music;
static bool musicLoaded;
static char trackName[160];

static const char *PLAYLIST[] = {
    "C:/Users/User/Music/Solo Piano/1. Einaudi_ Le Onde.mp3",
    "C:/Users/User/Music/Solo Piano/2. Einaudi_ Nefeli.mp3",
    "C:/Users/User/Music/Solo Piano/3. Einaudi_ Melodia Africana III.mp3",
    "C:/Users/User/Music/Solo Piano/4. Memory One.mp3",
    "C:/Users/User/Music/Solo Piano/5. Einaudi_ I Giorni.mp3",
    "C:/Users/User/Music/Solo Piano/6. Einaudi_ In Un'Altra Vita.mp3",
    "C:/Users/User/Music/Solo Piano/7. Una Mattina.mp3",
    "C:/Users/User/Music/Solo Piano/8. Nuvole Bianche.mp3",
    "C:/Users/User/Music/Solo Piano/9. Monday.mp3",
    "C:/Users/User/Music/Solo Piano/10. Jay.mp3",
    "C:/Users/User/Music/Solo Piano/11. Einaudi_ Seven Days Walking _ Day 2_ Birdsong.mp3",
    "C:/Users/User/Music/Solo Piano/12. Adieux.mp3",
    "C:/Users/User/Music/Solo Piano/14. Experience - Solo Piano.mp3",
    "C:/Users/User/Music/Solo Piano/15. Luminous.mp3",
    "C:/Users/User/Music/Solo Piano/16. Einaudi_ Elegy For The Arctic.mp3",
    "C:/Users/User/Music/Solo Piano/17. Einaudi_ The Snow Prelude No. 2.mp3"
};
#define PLAYLIST_COUNT (int)(sizeof(PLAYLIST)/sizeof(PLAYLIST[0]))
static int trackIndex = -1;

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
    FocusAudioNextTrack();
}

void FocusAudioSetEnabled(bool enabled)
{
    if (!initialized) return;
    if (enabled)
    {
        if (musicLoaded) ResumeMusicStream(music);
        else ResumeAudioStream(stream);
    }
    else
    {
        if (musicLoaded) PauseMusicStream(music);
        else PauseAudioStream(stream);
    }
}

static void LoadTrack(int index)
{
    if (musicLoaded)
    {
        UnloadMusicStream(music);
        musicLoaded = false;
        trackName[0] = '\0';
    }
    if (index < 0 || index >= PLAYLIST_COUNT) return;
    if (!FileExists(PLAYLIST[index]))
    {
        snprintf(trackName, sizeof(trackName), "Procedural Lofi");
        return;
    }
    music = LoadMusicStream(PLAYLIST[index]);
    if (music.stream.buffer != NULL)
    {
        musicLoaded = true;
        const char *name = strrchr(PLAYLIST[index], '/');
        name = name ? name + 1 : PLAYLIST[index];
        snprintf(trackName, sizeof(trackName), "%s", name);
        SetMusicVolume(music, 0.32f);
        PlayMusicStream(music);
    }
    else snprintf(trackName, sizeof(trackName), "Procedural Lofi");
}

void FocusAudioNextTrack(void)
{
    int next;
    do { next = GetRandomValue(0, PLAYLIST_COUNT - 1); } while (next == trackIndex && PLAYLIST_COUNT > 1);
    trackIndex = next;
    LoadTrack(trackIndex);
}

void FocusAudioSetVolume(float volume)
{
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    if (musicLoaded) SetMusicVolume(music, volume);
    if (initialized) SetAudioStreamVolume(stream, volume * 0.68f);
}

const char *FocusAudioTrackName(void)
{
    return trackName[0] ? trackName : "Procedural Lofi";
}

void FocusAudioUpdate(bool enabled)
{
    if (!initialized || !enabled) return;
    if (musicLoaded)
    {
        UpdateMusicStream(music);
        if (GetMusicTimePlayed(music) >= GetMusicTimeLength(music) - 0.25f)
            FocusAudioNextTrack();
        if (enabled) ResumeMusicStream(music);
        return;
    }
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
    if (musicLoaded) { UnloadMusicStream(music); musicLoaded = false; }
    if (!initialized) return;
    UnloadAudioStream(stream);
    initialized = false;
    CloseAudioDevice();
}
