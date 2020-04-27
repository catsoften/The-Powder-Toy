# CRBN
### Carbon Dust

![Element](https://i.imgur.com/qmQXsTk.gif)

*Superconducting CRBN* wire.

**Description:**  *Carbon dust. Turns into RDMD under heat and pressure, superconducts when cold.*

A light conductive powder. Sublimes at 3600 C into CO2. If temperature is greater than 2500 C and pressure greater than 100 there's a chance CRBN will condense into RDMD. When mixed with molten IRON will convert it into molten METL. Burns quickly. Transparent to NEUT but slows them down.

If it's temperature < 100 K it acts as a superconductor (like INST, but without the need for PSCN or NSCN). In order to superconduct, the point that recieves the SPRK must be touching *at most* 2 other CRBN (no thick wires). However, the middle portion can be as thick as you want. CRBN will accept SPRK from PSCN and NSCN, but not both at the same time.

If tmp2 > 0, CRBN will "act" like a solid, not moving. You can also make "solid" CRBN by mixing CRBN with PSTE. This makes it useful for liquid filters and wires.

Reactions:
- Purifies WATR into DSTW and salt water into either DSTW or SALT.
- When burnt with H2 turns into GAS (Kind of useless since the GAS instantly ignites)
- Randomly combines with H2 into GAS when over 10 C
- When burnt produces CO2
- When touches PSTE absorbs the PSTE and turns solid

## Technical
#### Properties
**life:** Used for SPRK, same as other conductors

**tmp:** Used to randomize powder color, default from 0 to 6

**tmp2:** Should it act solid?