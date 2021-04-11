# ONEWAY WALL
### One-Way Wall

![Wall](https://i.imgur.com/UBTWN7x.gif)

**Description:**  *One way wall, use line tool to set direction.*

Use the line tool to set the direction of the one way wall (like FAN), it will round to nearest direction of the following: (UP, DOWN, LEFT, RIGHT). Particles then are only allowed to move in that direction through the wall. Particles already in the wall can move freely in any direction, however.

To be considered moving in the direction, the velocity component must be greatest in that direction by a factor of 1.1 (ie, if moving up, vy must be negative and `abs(vy) > 1.1 * abs(vx)`)

When copy-pasting and rotating the selection, the direction of the one-way wall WILL update :)