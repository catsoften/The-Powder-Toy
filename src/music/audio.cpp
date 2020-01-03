#include "music/audio.h"
#include "music/music.h"

#include <cmath>
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL2/SDL.h>
#include <iostream>

// See https://stackoverflow.com/questions/10110905/simple-sound-wave-generator-with-sdl-in-c

Sound::Sound(double freq)
    : m_sineFreq(freq),
      m_sampleFreq(44100),
      m_samplesPerSine(m_sampleFreq / m_sineFreq),
      m_samplePos(0)
{
    SDL_AudioSpec wantSpec, haveSpec;

    SDL_zero(wantSpec);
    wantSpec.freq = m_sampleFreq;
    wantSpec.format = AUDIO_U8;
    wantSpec.channels = 1;
    wantSpec.samples = 512;
    wantSpec.callback = SDLAudioCallback;
    wantSpec.userdata = this;

    m_device = SDL_OpenAudioDevice(NULL, 0, &wantSpec, &haveSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (m_device == 0 || m_device > 3) {
        auto i = NOTE::sounds.begin();
        while (i != NOTE::sounds.end()) {
            (*i)->stop();
            delete *i;
            i = NOTE::sounds.erase(i);
        }
        if (m_device == 0)
            m_device = SDL_OpenAudioDevice(NULL, 0, &wantSpec, &haveSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    }
    if (m_device == 0)
        std::cout << "Failed to open audio: " << SDL_GetError() << std::endl;
}

Sound::~Sound() {
    if (!this) return;
    stop();
    SDL_CloseAudioDevice(m_device);
}

void Sound::play() {
    if(!this) return;
    SDL_PauseAudioDevice(m_device, 0);
}

void Sound::stop() {
    if (!this) return;
    SDL_PauseAudioDevice(m_device, 1);
}

void Sound::SDLAudioCallback(void *data, Uint8 *buffer, int length) {
    Sound *sound = reinterpret_cast<Sound*>(data);
    sound->m_samplesPerSine = sound->m_sampleFreq / sound->m_sineFreq;
    float multi = 1;
    float tone1, tone2, tone3;

    for(int i = 0; i < length; ++i) {
        int cback = sound->callbacks;

        // Electric synth (Causes static and popping noises during quick notes)
        multi = 1;
        if (cback > SOUND_CALLBACKS_TO_STOP) // Cut off sound after a certain amount of time
            multi = 0;
        // if (cback < 10)       // 0 - 10 is attack
        //     multi = cback / 10.0f;
        // else if (cback < 25)  // 10 - 25 = decay
        //     multi = 0.5f + 1.25f * cback / 25.0f;
        // else if (cback < 50)  // 25 - 50 = hold
        //     multi = 0.5;
        // else if (cback < 200) // 50 - 200 = release
        //     multi = 2.5f * (200 - cback) / 1000.0f;
        // else                  // Note over
        //     multi = 0;

        if (multi > 1) multi = 1;
        if (multi < 0) multi = 0;

        tone1 = multi * (std::sin(sound->m_samplePos / sound->m_samplesPerSine * M_PI * 2) + 1) * 127.4;
        tone2 = 0.50 * multi * (std::sin(sound->m_samplePos / sound->m_samplesPerSine * M_PI * 2 + M_PI / 2) + 1) * 127.4;
        tone3 = 0.25 * multi * (std::sin(sound->m_samplePos / sound->m_samplesPerSine * M_PI * 2 + M_PI / 4) + 1) * 127.4;

        buffer[i] = (0.7 * tone1 +  0.2 * tone2 + 0.3 * tone3);
        ++sound->m_samplePos;
    }

    ++sound->callbacks;
}