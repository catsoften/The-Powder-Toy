
# I-WALL

### Indestructible Wall

![Wall](https://i.imgur.com/RGjG4mU.gif)

**Description:**  *Prevents particles inside from being destroyed.*

Particles inside the wall cannot be destroyed or change state. The sole exception is spark, particles can change into SPRK and SPRK can change back to its ctype. Other than that, particles cannot melt or be destroyed (including energy particles reaching life 0, BOMB, DEST, CRAY deleters, etc...).

The wall could be useful for electronics that need to operate in dangerous conditions, ie lasers or DEST cannons and you don't want them melting. Most explosives inside the wall will trigger their explosion code, but won't be consumed, resulting in an infinite explosion.

The replace tool and erase tool work inside the wall.
