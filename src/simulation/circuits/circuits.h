#ifndef CIRCUITS_H
#define CIRCUITS_H

#include "simulation/CoordStack.h"
#include "simulation/ElementCommon.h"
#include "simulation/circuits/framework.h"
#include <vector>

class Circuit {
private:
    Simulation * sim;
    short skeleton_map[YRES][XRES];
public:
    Circuit(const coord_vec &skeleton, Simulation *sim);

    void generate(const coord_vec &skeleton);
    void add_branch_from_skeleton(const coord_vec &skeleton, int x, int y, int start_node);
};

class Branch {
public:
    // Notes on polarity:
    // Voltage: going from node1 to node2 increases by this voltage, ie
    //   NODE1 ----- - | + ------ NODE2

    const int node1, node2;
    const std::vector<int> ids;
    const float resistance, voltage_gain;

    Branch(int node1, int node2, const std::vector<int> &ids, float resistance, float voltage_gain) :
        node1(node1), node2(node2), ids(ids), resistance(resistance), voltage_gain(voltage_gain) {}
};

// coord_vec floodfill(Simulation *sim, Particle *parts, int x, int y);

#endif