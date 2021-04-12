# ERAY
### Property Setter Ray

![Element](https://i.imgur.com/5EUDlTF.gif)

**Description:**  *Property setter ray. Copies all its properties onto the first particle it hits.*

Like ARAY, fires opposite from where its sparked. Can only be activated by SPRK with life = 3. Behaviour varies depending on the ctype of the SPRK:

**PSCN:** Adds current properties to the target. For example, if ERAY had a life of 100, it would add 100 life to whatever it hits.

**NSCN:** Subtracts current properties from the target. For example, if ERAY had a life of 100, it would subtract 100 life to whatever it hits.

**INWR:** Sets all properties to target, but ignores any properties that are set to 0. For example, if ERAY had tmp = 1, tmp2 = 5 and everything else set to 0, it would only set tmp and tmp2 of the target element.

**Anything else:** Copies all properties exactly, including properties that are set to 0.

## Technical
#### Properties
All its properties are copied except for flags, vx and vy.
