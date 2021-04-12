# BLOD

![Element](https://i.imgur.com/dGQW0r8.gif)

**Description:**  *Blood. Stains particles, clots when still.*

A red liquid that stains powders and solids it touches red. If it touches liquid will stain it a more diluted red. When blood remains still for a while it will clot into a solid form. Blood will absorb oxygen and share it with its neighbours (Not when clotted). Clotted blood does not have any dynamic interactions (such as staining, absorbing oxygen, etc...)

Blood freezes into blood ice. Blood, when boiled, turns into red water vapor, with a small chance of releasing BRMT. Blood has a very small chance to kill BCTR and VIRS, which is quite useless. BLOD is dissolved by SOAP.

## Technical

#### Properties
**life:** How much oxygen stored, from 0 to 100. Less oxygenated blood is darker. 

**tmp:** How many particles it can stain, it can stain about twice as many liquid particles as solid.

**tmp2:** Clot counter, when it reaches a certain value (180 I think) the blood "clots". Increments when absolute value of each velocity component < 0.1 and is touching NONE.