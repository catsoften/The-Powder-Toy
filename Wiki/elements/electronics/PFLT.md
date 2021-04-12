# PFLT
### Powered Filter

![Element](https://i.imgur.com/csfiPGs.gif)

**Description:**  *Powered filter. Toggle with PSCN / NSCN, use COPR / ZINC to change tmp.*

Just like regular FILT, but can be turned on and off. Sparking with COPR will increase the tmp of all touching PFLT by 1 (at most 100), sparking with ZINC will decrease (at least 0). It's recommended to only use 1 px of COPR or ZINC to do this since PFLT can detect SPRK from 2 px away.

## Technical
#### Properties
**ctype:** Same as ctype in FILT

**life:** Used for on off state (10 = ON, 0 = PFF)

**tmp:** Same as tmp in FILT

**tmp2:** "Cooldown" before changing tmp again