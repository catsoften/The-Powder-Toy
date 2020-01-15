# SHRD
### Shredder

![Element](https://i.imgur.com/V7PVreM.gif)

**Description:**  *Shredder. Breaks solids and certain powders when they enter.*

A powered shredder, breaks a lot of solids that touch it. All particles can go through SHRD. Here are the transitions for normal breaking:

```
WOOD        -> SAWD
COAL        -> BCOL
PLNT / VINE -> SEED
BIRD / FISH -> BLOD (Oxygenated)
BEE / ANT / SPDR -> BCTR (Dead)
AERO        -> GEL
IRON / METL / MMSH / BMTL -> BRMT
GLAS / FIBR -> BGLA
CRMC        -> CLST
RCRT        -> CNCT
CNCT        -> STNE
WIFI / ARAY / DRAY / CRAY / EMP / SWCH -> BREL
VIBR        -> BVBR
FUSE        -> FSEP
CRBN (solid) -> CRBN (powder)
ROCK (solid) -> ROCK (powder)
Any HighPressureTransition property set -> That
```

In addition, there are some additional transitions:
```
SAWD -> Broken sawdust
PAPR -> Broken paper
MONY -> Broken paper
Conducting solids not mentioned above -> broken solid (fuses when sparked)
```

## Technical
#### Properties
**life:** Is it on? 0 = OFF, 10 = ON

**tmp:** Used for animation
