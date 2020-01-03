#ifndef MUSIC_H
#define MUSIC_H

#include <unordered_map>
#include <vector>
#include "music/audio.h"

namespace NOTE {
	const int LIFE_CHUNK = 6;
	const int MAX_NOTES_AT_SAME_TIME = 2; // If this >= 3 then game randomly crashes sometimes (probably # of channels)
	extern int next_audio_device; // How many notes are playing this frame? Used to cycle audiodevices

	extern std::vector<Sound*> sounds;
	extern std::unordered_map<int, float> key_map;

    float get_frequency_from_key(int key);
}

#endif