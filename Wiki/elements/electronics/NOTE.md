# NOTE
### Noteblock

![Element](https://i.imgur.com/snDC0EV.png)

*An ARAY piano covering every possible note, reading from layered photon storage*

**Description:**  Plays a note when sparked, tmp corresponds to piano key. (Earrape warning)

Plays a note when SPRKed or hit by BRAY (If hit by BRAY diagonals don't count). The note will last for a length of time set by life.

Due to limitations of SDL2 Audio API, there might be popping or crackling sounds (mostly with the violin, rest are fine), and 
only 2 sounds can play at once because I can't get audio mixing to sound good.

A demo video [here](https://www.youtube.com/watch?v=2dhK1PLomFs) (Bad Apple). The current mod has a smoother sound but still has occasional popping noises.

## Technical
#### Properties
**life:** Length of the note, in number of audio callbacks. Audio callbacks don't exactly correspond to frames, so 
if your FPS is low you might experience gaps in short sounds (if you keep playing a short sound). Default is 30.

**tmp:** Note to play. See [here](https://en.wikipedia.org/wiki/Piano_key_frequencies) for a list of piano frequencies, the key number column corresponds to the tmp value to play that note.

**tmp2:** Instrument to play:
```
SQUARE = 0;    Square wave
TRIANGLE = 1;  Triangle wave
SAW = 2;       Saw wave
SINE = 3;      Sine wave
VIOLIN = 4;    Stolen violin sample
```

There is also a lua api to play sounds:
```
tpt.play_sound(note_id or frequency, length=30, instrument=0, interpret as node_id=0)
```