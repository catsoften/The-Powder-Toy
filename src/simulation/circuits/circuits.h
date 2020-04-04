#ifndef CIRCUITS_H
#define CIRCUITS_H

#include "simulation/CoordStack.h"
#include "simulation/ElementCommon.h"
#include "simulation/circuits/framework.h"

#include "eigen/Core"
#include "eigen/Dense"
#include "eigen/SparseCholesky"

#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

#define BASE_RSPK_LIFE 4
#define FORCE_RECALC_EVERY_N_FRAMES 20
#define INTEGRATION_TIMESTEP 0.016666666666666666 // 1 frame = 1/60 s

#define MAX_CAPACITOR_CURRENT 10000.0f
#define MAX_CAPACITOR_REVERSE_CURRENT 1.0f

class Branch;
class Circuit;

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
class Circuit {
private:
    Simulation * sim;
    short skeleton_map[YRES][XRES];
    char immutable_nodes[YRES][XRES]; // 1 = directly adjacent, 2 = diagonally adjacent
    bool recalc_next_frame = false; // Flag for regeneration
    bool contains_dynamic = false;  // Contains dynamic components such as capacitors
    bool solution_computed = false; // Already computed solution for non-dynamic systems
    int startx, starty;

    // These can contain duplicate node-node id pairs, and follow same order, ie if connection map
    // for node 5 at index 2 is node 3, then the branch at node 5 index 2 is for node 3
    // Note: duplicate node-node pairs indicate multiple connections from 1 node to another
    // and represent different branches
    std::unordered_map<int, std::vector<int> > connection_map; // Node ID: Ids that connect
    std::unordered_map<int, std::vector<Branch *> > branch_map; // Node ID: branches with start_node = id
    std::unordered_map<int, std::vector<Branch *> > floating_branches; // Node ID, branches with start_node = id
    std::vector<Branch *> branch_cache; // Stores pointers so we can delete them later
    std::vector<int> global_rspk_ids;
    std::set<int> ground_nodes; // Reference for nodes = 0 V

    void trim_adjacent_nodes(const coord_vec &nodes);
    void add_branch_from_skeleton(const coord_vec &skeleton, int x, int y, int start_node, int sx, int sy);

    static bool is_dynamic_particle(int type) {
        return type == PT_PTCT || type == PT_NTCT || type == PT_SWCH || type == PT_CAPR || type == PT_INDC; }
    static bool is_voltage_source(int type) {
        return type == PT_VOLT || type == PT_CAPR; }
public:
    void generate();
    void solve(bool allow_recursion=true);
    void update_sim();
    void reset_effective_resistances();
    void flag_recalc() { recalc_next_frame = true, solution_computed = false; }
    bool should_recalc() { return recalc_next_frame; }
    void reset();
    void debug();

    size_t branch_cache_size() { return branch_cache.size(); }
    const std::vector<int> &get_global_rspk_ids() { return global_rspk_ids; }

    Circuit(int x, int y, Simulation *sim);
    Circuit(const Circuit &other);
    ~Circuit();
};

// A line between 2 nodes. I have no idea what they're actually called
// so I'll call them branches (or maybe edges)
class Branch {
public:
    /**
     * Resistance is base resistance (only resistors)
     * Effective resistance is dynamic, base is static
     * 
     * Notes on polarity:
     * Voltage: going from node1 to node2 drops by this voltage, ie
     *   NODE1 ----- + | - ------ NODE2
     * is positive voltage
     * 
     * Current: flows from node1 to node2 (positive)
     *   NODE1 -----> NODE2 (positive)
     * 
     * Dioide: going from node1 to node2 is positive_diode
     *   NODE1 ----- PSCN NSCN ------ NODE2
     */

    const int node1, node2;
    const std::vector<int> ids;
    const std::vector<int> rspk_ids;
    const std::vector<int> switches;
    double resistance, voltage_gain, current_gain, base_resistance, current=0.0;
    const int diode; // 0 = no diode, 1 = positive, -1 = negative
    const int node1_id, node2_id;
    double V1, V2, prev_step=0.0;
    
    bool recompute_switches = true;

    Branch(int node1, int node2, const std::vector<int> &ids, 
            const std::vector<int> &rspk_ids, const std::vector<int> &switch_ids,
            double resistance, double voltage_gain, double current_gain, int diode, int id1, int id2) :
        node1(node1), node2(node2), ids(ids), rspk_ids(rspk_ids), switches(switch_ids), resistance(resistance),
        voltage_gain(voltage_gain), current_gain(current_gain), base_resistance(resistance), diode(diode),
        node1_id(id1), node2_id(id2) {}
    
    void setSpecialType(bool isCapacitor, bool isInductor);
    void print();
    void reachedDivergenceCondition();

    void computeDynamicResistances(Simulation * sim);
    void computeDynamicVoltages(Simulation * sim);
    void computeDynamicCurrents(Simulation * sim);

    bool obeysOhmsLaw() { return obeys_ohms_law; };
    bool isInductor() { return is_inductor; }
    bool isCapacitor() { return is_capacitor; }
private:
    bool switches_on = false;
    bool is_capacitor = false;
    bool is_inductor = false;
    bool obeys_ohms_law = true;

    bool switchesOn(Simulation * sim);
};

#endif