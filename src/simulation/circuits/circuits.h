#ifndef CIRCUITS_H
#define CIRCUITS_H

#include "simulation/CoordStack.h"
#include "simulation/ElementCommon.h"
#include "simulation/circuits/framework.h"

#include <vector>
#include <set>
#include <unordered_map>

class Branch;
class Circuit {
private:
    Simulation * sim;
    short skeleton_map[YRES][XRES];
    char immutable_nodes[YRES][XRES]; // 1 = directly adjacent, 2 = diagonally adjacent

    // These can contain duplicate node-node id pairs, and follow same order, ie if connection map
    // for node 5 at index 2 is node 3, then the branch at node 5 index 2 is for node 3
    // Note: duplicate node-node pairs indicate multiple connections from 1 node to another
    // and represent different branches
    std::unordered_map<int, std::vector<int> > connection_map; // Node ID: Ids that connect
    std::unordered_map<int, std::vector<Branch *> > branch_map; // Node ID: branches with start_node = id
    std::vector<Branch *> branch_cache; // Stores pointers so we can delete them later
    std::set<int> ground_nodes; // Reference for nodes = 0 V

    void generate(const coord_vec &skeleton);
    void trim_adjacent_nodes(const coord_vec &nodes);
    void add_branch_from_skeleton(const coord_vec &skeleton, int x, int y, int start_node, int sx, int sy);
    void solve(bool allow_recursion=true);
    void debug();
public:
    Circuit(const coord_vec &skeleton, Simulation *sim);
    ~Circuit();
};

// A line between 2 nodes. I have no idea what they're actually called
// so I'll call them branches (or maybe edges)
class Branch {
public:
    /**
     * Notes on polarity:
     * Voltage: going from node1 to node2 increases by this voltage, ie
     *   NODE1 ----- - | + ------ NODE2
     * is positive voltage
     * 
     * Dioide: going from node1 to node2 is positive_diode
     *   NODE1 ----- PSCN NSCN ------ NODE2
     */

    const int node1, node2;
    const std::vector<int> ids;
    const std::vector<int> switches;
    const float resistance, voltage_gain;
    const int positive_diodes, negative_diodes;

    Branch(int node1, int node2, const std::vector<int> &ids, const std::vector<int> &switch_ids,
            float resistance, float voltage_gain, int pdiode, int ndiode) :
        node1(node1), node2(node2), ids(ids), switches(switch_ids), resistance(resistance),
        voltage_gain(voltage_gain), positive_diodes(pdiode), negative_diodes(ndiode) {}
};
#endif