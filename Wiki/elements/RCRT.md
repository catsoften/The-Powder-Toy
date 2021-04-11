## RCRT 
### Reinforced concrete

![Element](https://i.imgur.com/cxRzXxs.gif)

**Description:** *Reinforced concrete. Remains in place near supporting solids.*

Reinforced concrete, stacks like CNCT and no powders or liquids can go through (including DEST). Melts 100 degrees higher than CNCT (at 1223 K). RCRT can be made from solidifed CEMT.

Does not move if either:
- Touching a supporting solid
- Touching another particle touching a support solid
- Touching another particle that's touching another particle touching a support solid...
- The max extension length is 5 pixels. Any particles further will simply fall down.

If a particle is moving too fast it won't connect.


## Technical

#### Properties
**tmp:** How far it is from a solid, 1 = touching, 2 = touching another particle touching a solid, ...

**tmp2:** Set to 10 everytime a group is touching a solid, used for checking if still attached to a supporting solid (avoid floating groups). Counts down every frame, if the solid support disappears will count down to 0, then the RCRT will fall.