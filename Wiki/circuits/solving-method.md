
# Circuit Solving Method

  

## 0. Identify circuits

  

## 1. Identify circuit nodes and edges (branches)

First we take the circuit starting point we identified and do a floodfill to get all conductors connected to the starting point.  The flood fill is just a standard stack based floodfill, where each pixel can fill a new pixel based on the following rules:

```
1. New coordinate hasn't been visited
2. Is a valid conductor
3. Isn't conducting from SWCH to PSCN or NSCN
4. Isn't conducting to INWR from anything other than INWR, a terminal or GRND
5. Isn't conducting from INWR to anything other than INWR, a terminal, GRND
```

Terminals:
```
Positive: PSCN, COPR
Negative: NSCN, ZINC
```

COPR / ZINC can be used as low-resistance alternatives to PSCN / NSCN

The last rule prevents RSPK blocking SPRK from toggling SWCH. Coordinates are added to a coordinate vector and returned.

---

Next, we apply the Zhang-Suen algorithm for image thinning to create a skeleton of the circuit:

![Skeletonization process](https://i.imgur.com/pyOXJlX.png)


Next, we identify nodes by the following criteria (blue in above diagram). We consider "nodes" as junctions where parts of the skeleton branch, as all wires (aside from superconductors) have resistance, so nodes can't simply be placed between resistors. Rather, we consider the entire wire as one large resistor (Other components detailed later).

Furthermore, we would also like the points beside voltage sources such as capacitors and VOLT to be marked as nodes for nodal analysis.

This is documented in a comment in simulation/circuits/circuits.cpp, which is summarized as follows:

```
A node is any pixel of the skeleton that has more than 2 surrounding pixels that are also
part of the skeleton. A surrounding pixel is not counted if it is directly adjacent to 
another surrounding pixel that's on the diagonal.

A positive terminal to voltage source type, such as VOLT and CAPR is a node
A negative terminal to voltage source type, such as VOLT and CAPR is a node
PSCN to NSCN or NSCN to PSCN (a p-n junction diode)
The current pixel is a ground
```

![Valid node diagram](https://i.imgur.com/PdtHwar.png)

By convention, diagonals take priority over directly adjacent. This is because of the skeletonization algorithm: it's very unlikely (or maybe impossible) for a diagonal to end up between 2 directly adjacent pixels, but far more likely for a directly adjacent pixel to end up between 2 diagonals.

---

If you end now, you might notice with very organic shaped circuits there might be nodes that end up right next to each other, or groups of nodes in blobs. (For example, the following shape has 5 nodes:)

![Example of invalid node configuration](https://i.imgur.com/sLxYOPQ.png)

Obviously this is undesirable: we only want the center pixel to be marked as a node. We also do not want to remove nodes around voltage sources and the other special cases, so those are marked "immutable" and cannot be trimmed.

In the code, a node is immutable and directly adjacent to the target if the immutable_map = 1, or diagonally adjacent if = 2

---

Immutable nodes cannot be trimmed unless an immutable node is diagonally adjacent to its target AND is touching an immutable node directly adjacent to its target, in which the diagonal node is redundant:

![Redundant node diagram](https://i.imgur.com/9I9rERo.png)

As we trim the nodes, we assign every node a numeric ID starting from 2 (1 means part of skeleton, 0 means not part of skeleton).

To trim node clusters, we simply do a floodfill, take the average location of all nodes in the cluster and set that sole location to be a node, clearing all others. In the event the average location was NOT originally a node, the closest node in the cluster to the average location will be picked.

**Exceptions to trimming clusters using average location:** 
- **3 Nodes, all x or y values are the same (a line)**: 
For small 1 px circuits, a 3 px wide line will be considered as 3 nodes. However, if we average the result, only the center pixel will remain, but we wish for the edge pixels to be nodes (see diagram:)

![Desired trim](https://i.imgur.com/vI0KSKh.png)

The solution is to simply delete the center pixel.

- **Clusters of > 5 nodes:**
Clusters larger than 5 nodes are formed by 1 px meshes, ie:

![Large node cluster](https://i.imgur.com/npFCS9q.png)

The solution is to count only directly adjacent pixels when considering nodes: nodes with > 2 *directly* adjacent pixels are classified as nodes, others are not.

We choose > 5 pixels because 5 pixels is the number of pixels in a star arrangement that can be reduced to 1 node, whereas any higher would indicate a 1px mesh with *multiple* nodes.

---

Finding branches (interchangeably called edges) is simply floodfilling from nodes.

Starting at each node, we take priority over directly adjacent skeleton pixels and begin calling the add branch function described below. Next, we floodfill any diagonals that is NOT touching a directly adjacent skeleton pixel.

![Diagram of floodfill start](https://i.imgur.com/jFwHSws.png)

We call the floodfill function twice as sometimes the skeletonization algorithm creates 2 branches that merge a pixel before the node (as we trimmed the nodes). The efficiency decrease is negligible as the branch algorithm maps where its visited, so calling it twice when there is only 1 branch will cause it to terminate on the first pixel.

---
Upon finding another node that's not the start node, the floodfill terminates. Floodfill also terminates if its unable to find another pixel that hasn't been visited before.

Floodfill takes the following priority list:
```
New node cannot be within 1 px of start node:
1. Directly adjacent NODES
2. Directly adjacent pixels (any)
3. Diagonally adjacent NODES
4. Diagonally adjacent pixels (any)

Repeat, but new node can be within 1 px of start node:
1. Directly adjacent NODES
2. Directly adjacent pixels (any)
3. Diagonally adjacent NODES
4. Diagonally adjacent pixels (any)
```

This search order guarantees we hit most of the pixels in the skeleton while not skipping over nodes. We permeantly delete skeleton pixels as we traverse over them so other branches don't use them, the exception is pixels 1 px from a node, which may be traversed multiple times.

We first ignore pixels within 1 px of start node to avoid making circular loops around start, and force the algorithim to traverse further. But this skips over a pixel in artifical 1 px L shapes, so we traverse again.

If we find an end node, we add the branch to a branch map, otherwise we add it to a map of floating branches.

As we floodfill, we keep track of the following variables, the important ones are:
```
IDs of particles (Only directly on the skeleton)
ids          - Vector of ids of conductors
rspk_ids  - Vector of ids of RSPK
switches - Vector of ids of switches

total_resistance - Equivalent resistance of the branch (includes end node, but not start)
total_voltage     - Equivalent voltage source of the branch, this overrides resistance
current_voltage - Used for polarity measurements

diode_type     - 0 = branch is not a diode, 1 = positive diode, 2 = negative diode
polarity           - Track polarity of objects such as VOLT and diodes, 0, 1 = positive, -1 = negative
```

Polarity resets to 0 on non-terminal/[polarized element such as VOLT] particle, is 1 on positive terminal and -1 on negative terminal.

**Adding to total voltage:**
![Voltage polarity diagram](https://i.imgur.com/WKSG1Gg.png)

If none of the above conditions are met then the voltage source is improperly connected (ie PSCN - VOLT - PSCN) and will not contribute to the total voltage. By default VOLT has super high resistance, so if total_voltage is 0 (invalid voltage source), then the branch will be treated as a  high-value resistor. Groups of voltage sources are averaged, so connecting through a blob of 10 V VOLT sources won't output 100 V.

**Diode polarity:**
![Diode polarity chart](https://i.imgur.com/nKNvEou.png)

The diode value = 1 if the branch is a diode and allows positive current, = - 1 if it only allows negative current, or = 0 if not a diode at all.

**Current polarity convention:**
By convention, positive current goes from start_node to end_node, where start_node < end_node. If floodfill discovers this is not the case, it swaps the nodes, reverses rspk_ids and takes the negative of the voltage.

## 2. Solve circuit
Solving the circuit is done with nodal analysis. Initially all nodes are solved by setting the sum of currents into the node to 0 (KCL). For example, the equation for node1 in the following diagram is:

![Equation KCL](https://i.imgur.com/IkmeoUC.png)

Using ohm's law to determine the current through each resistor as the difference in voltage, and summing the currents to 0. The actual matrix is solved using the eigen linear algebra library.

**Ground:**
If the node is a ground node, the equation is simply node = 0 (ground = 0V).

**Switches:** 
Each branch has a "base resistance" and an "effective resistance". The base resistance is the resistance of non-dynamic parts, whereas the effective resistance considers dynamic parts such as switches. The resistance is updated before the nodal calculation to consider these values.

**Voltage sources:**
If the current node contains a branch that is a voltage source, KCL is carried out as normal. After the matrix is generated (but not solved), a supernode equation is used:

![Supernode equations](https://i.imgur.com/Mu0vtIh.png)

This is done by adding one of the rows of the matrix of the 2 ends of the branch of the voltage source into the other, then setting the other one to the supernode definition equation. Example:

![Supernode part 2](https://i.imgur.com/R27Gvol.png)

**Diodes:**
To save time doing newton approximations and solving diode equations, our diodes are *very* ideal:

![V-I diode graph](https://i.imgur.com/1o7LG3q.png)

The only realistic thing about our diodes is that it has a breakdown and forward voltage. For ease of use, breakdown is defined as -1000 V and forward as 0.7 V.

When diodes are present, after initially solving the matrix, the voltage across each diode is considered. If the voltage is in the right direction and does not meet V_forward or is the wrong polarity and does not meet V_breakdown, then the diode does not conduct, and gets a 10 gigaohm boost to its resistance. Otherwise, the diode is replaced with a -0.7 V voltage source. The matrix is re-computed once again (not recomputed again after this).

After solving, branches are assigned node voltages and currents, and floating branches are set to the voltage of the node they're connected to (no current).

## 3. Translate solution to Simulation


## 4.