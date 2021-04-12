# ANT

![Element](https://i.imgur.com/7Q3jdLw.gif)

**Description:**  *Ants, digs tunnels in soil and eats SUGR. Flammable.*

The main goal of the ANT AI is to dig tunnels. If it happens to come across SUGR, HONY, SWTR or SEED it will consume it, but it won't actively seek it out. Any SOIL near ANT will turn into "tunnel soil", that is it won't fall. Ants can dig into soil, making it darker. Darker soil is a "tunnel", and other particles can occupy the tunnel. Outside of the tunnel ants will randomly move around. Ants will turn around if they're digging and hit a non-SOIL particle.

Ant turns into SNOW below -5 C and into DUST above 50 C. If ant touches any non-HONY or SWTR liquid, it will simply disappear ("drown"). If there are too many particles stacked where the ant is it will disappear to avoid BHOL forming. 

The digging algorithim is a type of [turmite](https://en.wikipedia.org/wiki/Turmite) similar to Langton's ant but modified by the developer from the Wikipedia example. I have no idea what it is anymore but it looks nice.

## Technical
#### Properties
**tmp:** Dig direction, 0 = up, 1 = right, 2 = down, 3 = left.

**tmp2:** Ant state for the digging algorithim

**life:** Is the ant able to dig? 1 = yes, 0 = no