#ifndef CIRCUITS_H
#define CIRCUITS_H

#include "simulation/CoordStack.h"
#include "simulation/ElementCommon.h"
#include "simulation/circuits/framework.h"
#include "simulation/circuits/circuit_core.h"

#include "components/circuit.h"

#include "eigen/Core"
#include "eigen/Dense"
#include "eigen/SparseCholesky"

#include <vector>
#include <set>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <limits>


class Branch;
class Component;

extern Circuit * circuit_map[NPART]; // ID to Circuit *
extern std::vector<Circuit *> all_circuits;

namespace CIRCUITS {
    // Functions
    void addCircuit(int x, int y, Simulation * sim);
    void deleteCircuit(int i);
    void updateAllCircuits();
    void clearCircuits();
}

// Classes

class Component {
    enum COMPONENT_TYPE { LOGIC_GATE };
    COMPONENT_TYPE type;
    std::vector<int> input_nodes, output_nodes;
    const bool is_dynamic;
    
    Component(COMPONENT_TYPE type, bool is_dynamic, std::vector<int> input_nodes, std::vector<int> output_nodes):
        type(type), input_nodes(input_nodes), output_nodes(output_nodes), is_dynamic(is_dynamic) {};
};

#endif