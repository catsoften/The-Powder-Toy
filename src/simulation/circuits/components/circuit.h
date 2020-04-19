#ifndef CIRCUIT_CIRCUIT_CLASS
#define CIRCUIT_CIRCUIT_CLASS

#include "simulation/CoordStack.h"
#include "simulation/ElementCommon.h"
#include "simulation/circuits/circuit_core.h"
#include "simulation/circuits/framework.h"

#include <vector>
#include <unordered_map>

class Branch;

class Circuit {
    friend Branch;
private:
    Simulation *sim;
    short * skeleton_map = nullptr;
    char * immutable_nodes = nullptr; // 1 = directly adjacent, 2 = diagonally adjacent
    bool recalc_next_frame = false;   // Flag for regeneration
    bool contains_dynamic = false;    // Contains dynamic components such as capacitors (update every frame)
    bool solution_computed = false;   // Already computed solution for non-dynamic systems
    int highest_node_id = 0;
    int startx, starty;

    // Divergence checks
    bool requires_divergence_checking = false; // Steady state bounding for numeric integration (capacitors / inductors)
    bool computed_divergence = false;          // One time flag for creation
    Circuit *copy = nullptr;                   // Copy of circuit to solve for divergence check

    // These can contain duplicate node-node id pairs, and follow same order, ie if connection map
    // for node 5 at index 2 is node 3, then the branch at node 5 index 2 is for node 3
    // Note: duplicate node-node pairs indicate multiple connections from 1 node to another
    // and represent different branches
    std::unordered_map<NodeId, std::vector<NodeId> > connection_map;         // Node ID: Ids that connect
    std::unordered_map<NodeId, std::vector<Branch *> > branch_map;        // Node ID: branches with start_node = id
    std::unordered_map<NodeId, std::vector<Branch *> > floating_branches; // Node ID, branches with start_node = id
    std::vector<Branch *> branch_cache;                               // Stores pointers so we can delete them later
    std::vector<int> global_rspk_ids;
    std::unordered_map<NodeId, Volts> constrained_nodes; // Reference for nodes = 0 V

    void DoFloodfill(coord_vec &skeleton);
    void MarkNodes();
    void TrimAdjacentNodes(const coord_vec &nodes);
    void AddBranchFromSkeleton(const coord_vec &skeleton, int x, int y, int start_node, int sx, int sy);

    void AddImmutableNode(NodeId node_id, Pos position, bool is_diagonal_connection);

    void DeleteMaps();
public:
    void generate();
    void solve(bool allow_recursion = true);
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

#endif