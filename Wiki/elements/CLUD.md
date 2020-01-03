# CLUD

### Cloud

![Element](https://i.imgur.com/uBOrLQy.gif)

**Description:**  *Cloud. Weather depends on ctype. (SNOW / WATR / LIGH / LAVA)*

A gas that everything passes through (like PROT), spawns weather depending on ctype. Does not move or be affected by pressure, it's basically a solid even though it's TYPE_GAS. THDR can pass through CLUD without detonating.

#### Ctypes:

**WATR / DSTW:** Rain cloud, gray ish. Creates WATR rain.

**LIGH / THDR:** Thundercloud, dark gray. Rains, and randomly creates LIGH. Super rare chance to make BALI.

**SNOW / ICE:** Snow cloud, light gray. Creates SNOW.

**GLAS:** Glass cloud, rains broken glass. Redish.

**LAVA:** Rains lava (Ctype STNE). Redish.

**Anything else:** Normal white cloud.

Note that the temperature of the "rain" is the temperature of the cloud itself, so if you have a snow cloud at 22 C, it will spawn rain instead of SNOW. CLUD does not conduct temperature.


## Technical
#### Properties
**ctype:** See above