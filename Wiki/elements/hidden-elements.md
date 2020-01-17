## BOWR (Bowserinator)
A more destructive version of SING (or less), generates MAX heat, MAX pressure, MAX gravity, fusion, and lightning. And eats particles. This element is hidden.

## LASR (Laser)
Like BRAY, but randomly deletes particles around it and heats everything to MAX_TEMP. Disappears after 4 frames. Glows white and flares 
when particles hit it. JCB1 spawns this.

## MSSL (Missile)
Guided missile, tries to accelerate to its target x, y coordinates (determined by pavg0, pavg1). If it hits a solid / powder detonates 
(transforms into BOMB). Can go through gas and liquid without interruption. Leaves behind a smoke trail.

## LCSM (Liquid cesium)
Same as CESM, but liquid. Explodes at 1600 C. When it goes above 671 C it will 
start to glow and bubble a bit, and won't conduct electricity. Same reactions as CESM.

## BRKN (Broken)
Represents a generic broken form. Flammability is a bit broken. Mimics ctype's update and graphics. Broken paper 
cannot be colored. Broken stuff can only melt. Chance to fuse when sparked (this is a bug but it's called a feature)