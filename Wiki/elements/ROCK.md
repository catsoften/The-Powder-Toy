# ROCK

![Element](https://i.imgur.com/m9yi77Z.png)

**Description:** *Rock. Tougher than brick, erodes slowly from moving water. Can be melted and refined into metals.*

Rock spawns with a random "ORE" type as its ctype. 

| Ore     | Distribution |
| ----------- | ----------- |
| RDMD     | 2%       |
| PLUT   | 2%        |
| URAN   | 2%        |
| GOLD  | 2%        |
| IRON | 22%        |
| BMTL | 5%        |
| COAL | 5%        |
| TUNG | 2%        |
| STNE | 58%        |

Above 12 pressure it will "shatter" into its ore type. The following ores are changed:
```
COAL -> BCOL
RDMD -> PQRT
TUNG -> BRMT
Any other powder -> The powder
Any other gas    -> The gas
Any other liquid -> The liquid
```
Otherwise, it just turns into a rock powder with its ctype. There is a slight chance for shattering ROCK to release GAS or OIL.

If its ore type is meltable and the ROCK is above the ore element's melting point it will melt into molten [rock ctype]. Non-meltable ctypes will melt at 1000 C into molten ROCK.

Rock will slowly turn into powder if in contact with ICE (ICE erosion), or slowly disappear if touching any type of moving water. It can randomly turn into BRMT if in contact with O2, and ROCK turns WATR and DSTW into SLTW.

## Technical

#### Properties
**tmp:** Used for speckles

**ctype:** ORE Type

**tmp2:** If not 0 then it becomes a powder.