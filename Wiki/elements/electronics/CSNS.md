# CSNS
### Color Sensor

![Element](https://i.imgur.com/045xT4J.gif)

**Description:**  *Color sensor, creates a spark when something with its dcolor is nearby.*

Sparks when an element with its dcolor is nearby. It checks graphics functions too, so it can detect color differences in DEUT, QRTZ, CLST, FILT, etc... If tmp is set to 1 it ignores dcolor when considering colors to match. CSNS cannot detect other CSNS. Change tmp2 to change radius from 1 to 25 inclusive (in a square shape around it).

When considering color, CSNS will blend any alpha present, and ignore fire glow.

## Technical
#### Properties
**temp:** Margin of error for matching each color component (RGB), if < 0 C will make sure error is greater than temp for all components.

**tmp:** If set to 1 will ignore dcolor when matching

**tmp2:** Radius for matching, 1 - 25

**dcolour:** Color to search for.