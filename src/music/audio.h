#ifndef AUDIO_H
#define AUDIO_H

#include <SDL.h>
#include <SDL_audio.h>
#include <SDL2/SDL.h>

const int SOUND_CALLBACKS_TO_STOP = 100;

class Sound {
public:
    Sound(double freq=440.0f);
    ~Sound();
    void play();
    void stop();

    double m_sineFreq;
    double m_sampleFreq;
    double m_samplesPerSine;
    uint32_t m_samplePos;
    int callbacks = 0;
    
    void set_freq(double f) {
        m_sineFreq = f;
        callbacks = 0;
    }
private:
    static void SDLAudioCallback(void *data, Uint8 *buffer, int length);
    SDL_AudioDeviceID m_device;
};

#endif