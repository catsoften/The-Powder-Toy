# NOTE
### Noteblock

![Element](https://i.imgur.com/snDC0EV.png)

*An ARAY piano covering every possible note, reading from layered photon storage*

**Description:**  Plays a note when sparked, tmp corresponds to piano key. (Earrape warning)

Plays a note when SPRKed or hit by BRAY (If hit by BRAY diagonals don't count). The note will last for a few seconds. 

Due to limitations of SDL2 Audio API, there might be popping or crackling sounds, and the sound will be a flat sine wave (no fancy synth, it will sound *bad*). Also there can only be around 2 sounds playing at once, before NOTE cycles and changes the frequency.

A demo video [here](https://www.youtube.com/watch?v=2dhK1PLomFs) (Bad Apple). The current mod has a smoother sound but still has occasional 
popping noises.

## Technical
#### Properties
**tmp:** Note to play. See [here](https://en.wikipedia.org/wiki/Piano_key_frequencies) for a list of piano frequencies, the key number column corresponds to the tmp value to play that note.