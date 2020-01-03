# BCTR
### Bacteria

[Image here (warning, large!)](https://i.imgur.com/VEKNuCV.gif)

**Description:**  *Bacteria. Feed it sugar to grow, can evolve genes.*

A liquid like substance with a genetic code, that can eat and reproduce. Radiation such as PROT, ELEC and NEUT will cause it to mutate (lessened by radiation hardening gene). Dissolved by SOAP and SALT. May mutate ability to cure VIRS. If below min temp will freeze into ICE that thaws into dead bacteria, and if above max temp will ignite into FIRE. By default BCTR produces GAS when eating, but this may change if it mutates an alternative food source.

Bacteria have a lifespan determined by their metabolism, after they run out it will turn into "Dead BCTR" which loses all resistive abilities. If it has enough energy (from eating food) the BCTR will reproduce into whatever empty space it can around it. If there isn't any, it'll renew its own lifespan at old age. Upon reproducing there is a chance of mutating, where a random bit in the gene is flipped.

A secret gene, if all 32 bits in the gene (including the unused ones) toggle to 1 the BCTR will "evolve" into SING. Also BCTR kills STKM.

BCTR passes on its dcolour property to its children.

If you wish to engineer bacteria there is a link to an editor [here](https://hellomouse.net/blog/bctr.html). (If the link doesn't work the HTML file is also under /Wiki/editors in this repo) 


## Technical
#### Genes
The types of genes are as follows. Genes higher up on the list represent earlier bits in the ctype. Default all genes are 0.
```
4:  0000  - Resistance type (See below)
			1 = HEAT (and FIRE)
			2 = COLD
			3 = VIRS
			4 = SOAP / SALT
			5 = ACID
			6 = Being EATEN
			7 = Radiation
8:  0000  - Resistance value (0 - 15, 0 = no resistance)
12: 0000  - Food type (See below, default: produce GAS)
			(Not listed) = SUGR
			1 = YEST / DYST
			2 = PLNT / WOOD / SAWD
			3 = PHOT or BRAY (No byproduct)
			4 = NEUT / ELEC / PROT
			5 = BCTR
			6 = Thermal energy (Lowers surrounding temperature)
16: 0000  - Metabolism speed (0 - 15, 0 = no growth) not including other gene modifiers
	   	    Energy storage is inversely proportional to metabolism
20: 0000  - Spread vector
			(Not listed) - Default liquid
			1 - Stick to solids and powders
			2 - Float through the air
			3 - Randomly swap with liquid particles
			4 - Inject self DNA into other BCTR
23: 000   - Move speed (increased energy cost, 0 - 7)
26: 000   - Graphics (fuzziness?)
			(Not listed) - PMODE_BLUR
			1 - SPARK
			2 - BLOB
			3 - FLAT
27: 0     - Glow in the dark?
31: 00000 - Unused
```

#### Properties
**ctype:** Gene, represented by a 32 bit int. Note that the *actual* binary representation differs from the format shown, as the format shown reverses the order of the chunks. For example, suppose the following gene:
```
001-010
```
Which has properties 1 and 2. However, it's actually stored as
```
010100
```
Instead of 
```
001010
```
As you might expect.

**life:** Energy level, if this goes to 0 it dies. Increases with food, speed of decrease depends on metabolism, move speed and glow. Note that if the metabolism is 0 it will still decrease by 1.

**tmp:** Current "age", once it reaches the max age the BCTR dies.

**tmp2:** Is the BCTR dead?