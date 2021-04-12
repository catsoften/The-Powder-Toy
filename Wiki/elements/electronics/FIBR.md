# FIBR
### Fiber Optic Cable

![Element](https://i.imgur.com/Lc5iUFW.gif)

**Description:**  *Fiber optic cable. Moves photons, set tmp for transfer speed.*

Photons will follow fiber optic cables, randomly picking a new direction (no back tracking) if they can no longer travel along the current direction. Photons do not lose life while inside of FIBR. Breaks above 5 pressure into broken glass and melts at 1973.15 K into molten glass. Allows spark to settle.

Setting tmp can set transfer speed (0 - 50 pixels / frame), photons will preserve this velocity when returning. It's recommended to keep the tmp throughout FIBR constant since PHOT only uses the speed of the current FIBR pixel its on, skipping ahead that many. If tmp is set to 0 FIBR acts as a photon trap, freezing any photons inside. FIBR larger than 1 px may result in photons being trapped in a cycle inside forever.

## Technical
#### Properties
**tmp:** Photon transfer speed (in px / frame, 0 - 50)