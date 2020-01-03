#include "music/music.h"
#include <unordered_map>
#include <vector>

namespace NOTE {
	std::vector<Sound*> sounds(MAX_NOTES_AT_SAME_TIME);
	std::unordered_map<int, float> key_map;

	int next_audio_device = 0;
	
	// From https://en.wikipedia.org/wiki/Piano_key_frequencies
	float get_frequency_from_key(int key) {
		// Create the keymap if empty
		if (key_map.size() == 0) {
			key_map.insert(std::make_pair(108, 7902.133f));
			key_map.insert(std::make_pair(107, 7458.620f));
			key_map.insert(std::make_pair(106, 7040.000f));
			key_map.insert(std::make_pair(105, 6644.875f));
			key_map.insert(std::make_pair(104, 6271.927f));
			key_map.insert(std::make_pair(103, 5919.911f));
			key_map.insert(std::make_pair(102, 5587.652f));
			key_map.insert(std::make_pair(101, 5274.041f));
			key_map.insert(std::make_pair(100, 4978.032f));
			key_map.insert(std::make_pair(99, 4698.636f));
			key_map.insert(std::make_pair(98, 4434.922f));
			key_map.insert(std::make_pair(88, 4186.009f));
			key_map.insert(std::make_pair(87, 3951.066f));
			key_map.insert(std::make_pair(86, 3729.310f));
			key_map.insert(std::make_pair(85, 3520.000f));
			key_map.insert(std::make_pair(84, 3322.438f));
			key_map.insert(std::make_pair(83, 3135.963f));
			key_map.insert(std::make_pair(82, 2959.955f));
			key_map.insert(std::make_pair(81, 2793.826f));
			key_map.insert(std::make_pair(80, 2637.020f));
			key_map.insert(std::make_pair(79, 2489.016f));
			key_map.insert(std::make_pair(78, 2349.318f));
			key_map.insert(std::make_pair(77, 2217.461f));
			key_map.insert(std::make_pair(76, 2093.005f));
			key_map.insert(std::make_pair(75, 1975.533f));
			key_map.insert(std::make_pair(74, 1864.655f));
			key_map.insert(std::make_pair(73, 1760.000f));
			key_map.insert(std::make_pair(72, 1661.219f));
			key_map.insert(std::make_pair(71, 1567.982f));
			key_map.insert(std::make_pair(70, 1479.978f));
			key_map.insert(std::make_pair(69, 1396.913f));
			key_map.insert(std::make_pair(68, 1318.510f));
			key_map.insert(std::make_pair(67, 1244.508f));
			key_map.insert(std::make_pair(66, 1174.659f));
			key_map.insert(std::make_pair(65, 1108.731f));
			key_map.insert(std::make_pair(64, 1046.502f));
			key_map.insert(std::make_pair(63, 987.7666f));
			key_map.insert(std::make_pair(62, 932.3275f));
			key_map.insert(std::make_pair(61, 880.0000f));
			key_map.insert(std::make_pair(60, 830.6094f));
			key_map.insert(std::make_pair(59, 783.9909f));
			key_map.insert(std::make_pair(58, 739.9888f));
			key_map.insert(std::make_pair(57, 698.4565f));
			key_map.insert(std::make_pair(56, 659.2551f));
			key_map.insert(std::make_pair(55, 622.2540f));
			key_map.insert(std::make_pair(54, 587.3295f));
			key_map.insert(std::make_pair(53, 554.3653f));
			key_map.insert(std::make_pair(52, 523.2511f));
			key_map.insert(std::make_pair(51, 493.8833f));
			key_map.insert(std::make_pair(50, 466.1638f));
			key_map.insert(std::make_pair(49, 440.0000f));
			key_map.insert(std::make_pair(48, 415.3047f));
			key_map.insert(std::make_pair(47, 391.9954f));
			key_map.insert(std::make_pair(46, 369.9944f));
			key_map.insert(std::make_pair(45, 349.2282f));
			key_map.insert(std::make_pair(44, 329.6276f));
			key_map.insert(std::make_pair(43, 311.1270f));
			key_map.insert(std::make_pair(42, 293.6648f));
			key_map.insert(std::make_pair(41, 277.1826f));
			key_map.insert(std::make_pair(40, 261.6256f));
			key_map.insert(std::make_pair(39, 246.9417f));
			key_map.insert(std::make_pair(38, 233.0819f));
			key_map.insert(std::make_pair(37, 220.0000f));
			key_map.insert(std::make_pair(36, 207.6523f));
			key_map.insert(std::make_pair(35, 195.9977f));
			key_map.insert(std::make_pair(34, 184.9972f));
			key_map.insert(std::make_pair(33, 174.6141f));
			key_map.insert(std::make_pair(32, 164.8138f));
			key_map.insert(std::make_pair(31, 155.5635f));
			key_map.insert(std::make_pair(30, 146.8324f));
			key_map.insert(std::make_pair(29, 138.5913f));
			key_map.insert(std::make_pair(28, 130.8128f));
			key_map.insert(std::make_pair(27, 123.4708f));
			key_map.insert(std::make_pair(26, 116.5409f));
			key_map.insert(std::make_pair(25, 110.0000f));
			key_map.insert(std::make_pair(24, 103.8262f));
			key_map.insert(std::make_pair(23, 97.99886f));
			key_map.insert(std::make_pair(22, 92.49861f));
			key_map.insert(std::make_pair(21, 87.30706f));
			key_map.insert(std::make_pair(20, 82.40689f));
			key_map.insert(std::make_pair(19, 77.78175f));
			key_map.insert(std::make_pair(18, 73.41619f));
			key_map.insert(std::make_pair(17, 69.29566f));
			key_map.insert(std::make_pair(16, 65.40639f));
			key_map.insert(std::make_pair(15, 61.73541f));
			key_map.insert(std::make_pair(14, 58.27047f));
			key_map.insert(std::make_pair(13, 55.00000f));
			key_map.insert(std::make_pair(12, 51.91309f));
			key_map.insert(std::make_pair(11, 48.99943f));
			key_map.insert(std::make_pair(10, 46.24930f));
			key_map.insert(std::make_pair(9, 43.65353f));
			key_map.insert(std::make_pair(8, 41.20344f));
			key_map.insert(std::make_pair(7, 38.89087f));
			key_map.insert(std::make_pair(6, 36.70810f));
			key_map.insert(std::make_pair(5, 34.64783f));
			key_map.insert(std::make_pair(4, 32.70320f));
			key_map.insert(std::make_pair(3, 30.86771f));
			key_map.insert(std::make_pair(2, 29.13524f));
			key_map.insert(std::make_pair(1, 27.50000f));
			key_map.insert(std::make_pair(97, 25.95654f));
			key_map.insert(std::make_pair(96, 24.49971f));
			key_map.insert(std::make_pair(95, 23.12465f));
			key_map.insert(std::make_pair(94, 21.82676f));
			key_map.insert(std::make_pair(93, 20.60172f));
			key_map.insert(std::make_pair(92, 19.44544f));
			key_map.insert(std::make_pair(91, 18.35405f));
			key_map.insert(std::make_pair(90, 17.32391f));
			key_map.insert(std::make_pair(89, 16.35160f));
		}
		if (key_map.count(key) == 0)
			return 440; // A is default in case we screw up since it's default ID, DO NOT RETURN 0 will cause divide by 0 error
		return key_map[key];
	}
}