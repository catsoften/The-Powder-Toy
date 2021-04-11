# IRDM
### Iridium

**Description:**  *Iridium. Produces fire when sparked.*

Produces FIRE and sometimes EMBR of the same temperature as the IRDM when sparked. Cannot conduct to PSCN, but 
PSCN can conduct to IRDM. When touching oxygen and is > 1000 C, will begin glowing rainbow colors. IRDM is immune 
to ACID and CAUS, but molten salt > 2000 C will slowly dissolve IRDM.

## Technical
#### Properties
**life:** Used by SPRK

**tmp:** Incremented when oxygen > 1000 C is touching, capped at 300 for oxygen (but can be higher if set using console)

**tmp2:** Used to cycle colors, clamped between 250 and 750
