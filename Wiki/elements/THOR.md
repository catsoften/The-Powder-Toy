# THOR
### Thorium

![Element](https://i.imgur.com/agzd0s0.gif)

**Description:**  *Thorium. Creates heat with neutrons, still radioactive when molten.*

A radioactive metal that conducts SPRK slowly like WATR. Allows neutrons through. Melts at 1200.0 C. When touching a neutron has a 1/15 chance to spawn 2 more neutrons with the same temp as itself, heat itself by 130 C and reduce its own tmp by 1. Once THOR's tmp reaches 0 (default: starts at 100), THOR will become "depleted" and be unable to react with NEUT. In the depleted state, it will slowly crumble into RADN. The more depleted THOR is, the less it glows. This allows nuclear reactors to be built using realistic control rods and neutron reflectors.

Cannot recieve SPRK from ELEC, but can from anti-protons. When left alone below 37 C will slowly warm itself to 37 C (Making it, strangely, good for growing YEST). Collision with PROT with a velocity of greater than 5 causes THOR to change into PLUT. THOR maintains its properties even when in its molten state.

## Technical
#### Properties
**life:** Used for SPRK

**tmp:** Fuel value (starts at 100 by default, decrements)

**tmp2:** Number of neutrons to emit in the next frame, used for internal logic