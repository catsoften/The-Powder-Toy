# BALI
### Ball Lightning

![Element](https://i.imgur.com/SkvjNzT.gif)

*A BALI explosion destroying CRMC without damaging the surrounding elements*

**Description:**  Ball Lightning. Actually a macroelectron, releases energy onto its ctype.

A ball of energy that floats through matter (like PROT) and moves against air flow. Spawns with a random ctype and energy level, if life is low turns invisible, but once charged up will start glowing different colors depending on energy level. Randomly sparks conductors. Absorbs other ball lightning nearby and merges energy levels (ctype is the one with higher energy). Also charges energy from THDR or LIGH. Electrons slightly charge BALI.

After a random amount of time it will explode, turning everything of its ctype in a `life / 20` (Maxs out at 60px) radius around it into a glowing solid that quickly disintegrates. It will also release 60 pressure and a small amount of SMKE. 

Default ctypes it can spawn with are randomly picked from:
```
PT_PLNT, PT_IRON, PT_METL, PT_IRON, PT_TTAN, PT_GOLD, PT_GLAS, PT_QRTZ, PT_BRCK, PT_CRMC, PT_WOOD, PT_GOO, PT_SAND
```

Although you can set any ctype yourself.

## Technical
#### Properties
**ctype:** Ctype to destroy

**life:** Energy level

**tmp:** Countdown to explosion in frames

**tmp2:** If != 0 then the BALI will act as a solid glowing block that decays depending on tmp, used for the disintegration effect.