# FARADAY WALL
### Faraday Wall

![Wall](https://i.imgur.com/twkYnzk.gif)

**Description:**  *Blocks WIFI, ETRD, EMP, and portals inside a box of this wall.*

Inside a box drawn of this wall, WIFI, portals, ETRD and EMP will become isolated. Portals inside do not transfer to portals outside of the box, portals outside do not conduct to portals on the inside of the box, and EMP inside will not affect electronics outside (and vice versa).

A box does not need to have corners to work. Using the debug element (DEBG) with tmp=1 will show the current faraday ID of the region. Note that objects *inside* the wall itself are **NOT CONSIDERED** inside the box. Any object inside a faraday wall is considered part of "outside", or ID = 0.

There can be at most be 100 bounded regions of faraday wall, any more faraday regions will end up reusing IDs. Pressure, gravity, particles, etc... can pass through faraday wall.

As an extra feature, setting tmp2 of WIFI to anything other than 0 will grant extra channels. Ie, wifi with same channels but one has a non-zero tmp2, other has 0 tmp2, they will not connect.