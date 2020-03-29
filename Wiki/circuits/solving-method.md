
# Circuit Solving Method

  

## 0. Identify circuits

  

## 1. Identify circuit nodes

First we take the circuit starting point we identified and do a floodfill to get all conductors connected to the starting point.  The flood fill is just a standard stack based floodfill, where each pixel can fill a new pixel based on the following rules:

```
1. New coordinate hasn't been visited
2. Is a valid conductor
3. Isn't conducting from SWCH to PSCN or NSCN
```

The last rule prevents RSPK blocking SPRK from toggling SWCH. Coordinates are added to a coordinate vector and returned.

---

Next, we apply the Zhang-Suen algorithim for image thinning to create a skeleton of the circuit:

![Skeletonization process](https://i.imgur.com/pyOXJlX.png)


Next, we identify nodes by the following criteria (blue in above diagram). We consider "nodes" as junctions where parts of the skeleton branch, as all wires (aside from superconductors) have resistance, so nodes can't simply be placed between resistors. Rather, we consider the entire wire as one large resistor (Other components detailed later).

Furthermore, we would also like the points beside voltage sources such as capacitors and VOLT to be marked as nodes for nodal analysis.

This is documented in a comment in simulation/circuits/circuits.cpp, which is summarized as follows:

```
A node is any pixel of the skeleton that has more than 2 surrounding pixels that are also part of the skeleton. A surrounding pixel is not counted if it is directly adjacent to another surrounding pixel that's on the diagonal.

A PSCN to voltage source type, such as VOLT and CAPR is a node
A NSCN to voltage source type, such as VOLT and CAPR is a node
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



## 2. Solve circuit

  

## 3. Translate solution to Simulation

  

## 4.