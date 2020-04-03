# JUNC

![Really bad gif lol](https://i.imgur.com/Qi58Rvx.gifv)

**Description:**  *Crossover junction. Allows wires to cross without interference.*

Conducts like normal, except connections entering must be directly adjacent (not on a diagonal, can exit in any direction). 
After entering, JUNC can only connect in the direction it was entered (ie, if you entered from the top, the connection will go straight down).
Thus, circuits can overlap through junctions.

Note that since normal circuits can conduct through diagonals anyways, the junction needs to be at least a 5px star shape to prevent 
a horizontal circuit from conducting to a vertical one.

Junction can also be used to cross over SPRK, but only for 1 px wires (larger wires tend to cause SPRK loops).