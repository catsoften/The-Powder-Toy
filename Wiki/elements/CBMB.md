# CBMB
### Chronobomb

[See here for demo (large image)](https://imgur.com/hgxXPRL)

*CBMB Extending GBMB's explosion*

**Description:**  *Chrono bomb. Sticks to the first object it touches then produces a strong time dilation field.*

Like GBMB, but with time. Chronobombs are not affected by time dilation fields of any kind (including their own). When it touches an object, it will stick and slowdown time near it by a factor of 8, slowly decreasing to 0 in a radius of 10 cells around it. During this time, it will be green and have portal in particles.

After some amount of time, it will reverse, speeding up time by a factor of 4 in a radius of 10 cells around it, in which its colors will invert and it will have a portal out particle effect.

## Technical
#### Properties
**life:** Used for portal graphics

**ctype:** Used for portal graphics

**tmp2:** Used for countdown, if > 0 will countdown to 1 when it dies. Set to 250 when it touches something, if > 50 will slow down time.
