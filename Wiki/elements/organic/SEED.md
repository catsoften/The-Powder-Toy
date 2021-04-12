# SEED

![Element](https://i.imgur.com/WdvbXqq.gif)

**Description:**  *Seed, sprouts into trees when planted in soil.*

Seed will grow trees when in contact with SOIL (not on a diagonal). Trees will be oriented away from the current gravity mode (up for vertical, random direction for no gravity, and away from center for radial). The type of tree depends on the temperature of the SOIL, SOIL with temperature < 5 C will make pine trees, SOIL with temp < 30 C makes oak, and < 42 C makes palm trees. If there is any WOOD within 2 px of the SEED it will sprout into grass (a 1-3 px tall PLNT).

SEED turns into SNOW under -20 C and ignites instantly above 300 C. If seed is exposed to temps above 42 C it will "die" and be unable to sprout. SEED changes color like PLNT when heated or cooled.


## Technical
#### Properties
**tmp:** Used for speckle variation graphics

**tmp2:** The highest temperature the SEED has ever "felt".