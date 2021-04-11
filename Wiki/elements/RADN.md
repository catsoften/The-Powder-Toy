# RADN
### Radon

**Description:**  Radon, unstable gas. Decays with slow, hot neutrons.

Radon is a radioactive gas that freezes at -71 C (IRL it has a liquid state but that's not in this mod). Frozen RADN is orange 
and has a fluffy gas effect. Radon will naturally slowly decay neutrons, however if it is hit by a slow PROT / NEUT (< 1.7 velocity, 
temp > 1000 K) radon will fissile when hit. When radon runs out of tmp it will decay into POLO. 

Frozen RADN will not decay.

## Technical
Used tmp for decay instead of life, as ICE thawing resets life (and ICE has LIFE_DEC property)

#### Properties
**tmp:** Neutrons left, decays into POLO when = 0

**tmp2:** Whether to use FIRE_ADD or FIRE_BLEND