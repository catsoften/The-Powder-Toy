#include "simulation/circuits/circuits.h"
#include "gui/game/GameModel.h"
#include "simulation/ElementCommon.h"
#include "simulation/circuits/resistance.h"
#include "simulation/circuits/util.h"

#include <iomanip>
#include <iostream>

Circuit * circuit_map[NPART];
std::vector<Circuit *> all_circuits;

// Functions
void CIRCUITS::addCircuit(int x, int y, Simulation * sim) {
    int r = sim->photons[y][x];
    if (circuit_map[ID(r)])
        return; // Already part of a circuit

    Circuit * c = new Circuit(x, y, sim);
    if (!c->branch_cache_size()) {
        delete c;
        return;
    }
    all_circuits.push_back(c);
}

void CIRCUITS::deleteCircuit(int i) {
    // Why don't we put this in the destructor?
    // Doing so causes extreme lag as invalid circuits are deleted
    // but by reseting circuit_map this causes the invalid circuit to
    // be recreated next frame over and over (ie in blobs of VOLT)
    for (auto id : all_circuits[i]->get_global_rspk_ids())
        circuit_map[id] = nullptr;
    delete all_circuits[i];
    all_circuits.erase(all_circuits.begin() + i);
}

void CIRCUITS::updateAllCircuits() {
    for (int i = all_circuits.size() - 1; i >= 0; i--) {
        Circuit * c = all_circuits[i];
        if (c->should_recalc()) {
            c->reset();
            c->generate();
        }
        if (!c->branch_cache_size()) {
            deleteCircuit(i);
            continue;
        }
        c->reset_effective_resistances();
        c->solve();
        c->update_sim();
    }
}

void CIRCUITS::clearCircuits() {
    for (auto c : all_circuits)
        delete c;
    all_circuits.clear();
    std::fill(&circuit_map[0], &circuit_map[NPART], nullptr);
}

// Classes
/**
 * Mark nodes on the skeleton map, and create branches.
 * Nodes are numbered with an ID
 * starting from 2 (0 = nothing, 1 = wire, 2+ = node_id)
 */
void Circuit::generate() {
    coord_vec skeleton = floodfill(sim, sim->parts, startx, starty);
    for (auto &pos : skeleton) {
        int id = ID(sim->photons[pos.y][pos.x]);
        sim->parts[id].tmp = 0;
        circuit_map[id] = this;
        global_rspk_ids.push_back(id);
    }
    skeleton = coord_stack_to_skeleton(sim, skeleton);

    std::fill(&skeleton_map[0][0], &skeleton_map[YRES][0], 0);
    std::fill(&immutable_nodes[0][0], &immutable_nodes[YRES][0], 0);
    for (auto &pos : skeleton) {
        skeleton_map[pos.y][pos.x] = 1;
        sim->parts[ID(sim->photons[pos.y][pos.x])].tmp = 1;
    }

    // Node marking
    char count;
    short x, y;
    int node_id = 2, type;
    std::vector<pos> nodes;

    for (auto &pos : skeleton) {
        count = 0;
        x = pos.x, y = pos.y;
        type = TYP(sim->pmap[y][x]);

        if (skeleton_map[y][x] > 1) // Already marked as node
            continue;

        /**
         * Special junction (node) cases: NSCN or PSCN connected to any of the following:
         *      capacitor, voltage source, inductor
         * Or the node itself is GRND
         */
        if (type == GROUND_TYPE) {
            skeleton_map[y][x] = node_id;
            immutable_nodes[y][x] = 1;
            nodes.push_back(pos);
            node_id++;
        }
        else if (type == PT_INDC) { // Inductor doesn't need terminals
            for (int rx = -1; rx <= 1; rx++)
            for (int ry = -1; ry <= 1; ry++)
                if ((rx || ry) && skeleton_map[y + ry][x + rx] && TYP(sim->pmap[y + ry][x + rx]) != PT_INDC) {
                    skeleton_map[y + ry][x + rx] = node_id;
                    immutable_nodes[y + ry][x + rx] = (rx && ry) + 1;
                    auto npos = pos; npos.x = x + rx; npos.y = y + ry;
                    nodes.push_back(npos);
                    node_id++;
                }
        }
        else if (is_terminal(type)) {
            int t_is_silicon = type == PT_PSCN || type == PT_NSCN;
            int other_type = type == PT_PSCN ? PT_NSCN : PT_PSCN;

            // Prioritze directly adjacent CAPR / VOLT / other such
            for (int rx = -1; rx <= 1; rx++)
            for (int ry = -1; ry <= 1; ry++)
                if ((rx || ry) && (rx == 0 || ry == 0)) {
                    int t = TYP(sim->pmap[y + ry][x + rx]);
                    if (is_voltage_source(t) || (t_is_silicon && t == other_type) || is_chip(t)) {
                        skeleton_map[y][x] = node_id;
                        immutable_nodes[y][x] = (rx && ry) + 1;
                        nodes.push_back(pos);
                        node_id++;
                        goto end;
                    }
                }
            for (int rx = -1; rx <= 1; rx++)
            for (int ry = -1; ry <= 1; ry++)
                if ((rx || ry) && !(rx == 0 || ry == 0)) {
                    int t = TYP(sim->pmap[y + ry][x + rx]);
                    if (is_voltage_source(t) || (t_is_silicon && t == other_type) || is_chip(t)) {
                        skeleton_map[y][x] = node_id;
                        immutable_nodes[y][x] = (rx && ry) + 1;
                        nodes.push_back(pos);
                        node_id++;
                        goto end;
                    }
                }
            end:;
        }

        if (!can_be_node(ID(sim->pmap[y][x]), sim))
            continue;

        /**
         * Count surrounding disjoint connections, if > 2 then it must be a junction, ie:
         * #YY
         *   X##
         *   #
         * The pixel marked x has 3 connections going into it, even though it has 4 surrounding
         * pixels, as the surrounding pixels marked with Y are touching, so are only counted once.
         * By convention, the code below ignores directly adjacent pixels (up, down, left, right) if a diagonal
         * touching that pixel is already filled; this allows for more diagonal connections to be considered.
         */
        for (int rx = -1; rx <= 1; rx++)
        for (int ry = -1; ry <= 1; ry++)
            if ((rx || ry) && skeleton_map[y + ry][x + rx]) {
                if (!rx && ry && (
                    skeleton_map[y + ry][x - 1] ||
                    skeleton_map[y + ry][x + 1]
                )) continue;
                if (rx && !ry && (
                    skeleton_map[y + 1][x + rx] ||
                    skeleton_map[y - 1][x + rx]
                )) continue;
                count++;
            }
        if (count > 2) {
            skeleton_map[y][x] = node_id;
            nodes.push_back(pos);
            node_id++;
            continue;
        }
    }
    trim_adjacent_nodes(nodes);

    // Branch generation
    for (auto &pos : skeleton) {
        x = pos.x, y = pos.y;

        if (skeleton_map[y][x] > 1) {
            sim->parts[ID(sim->photons[y][x])].tmp = skeleton_map[y][x];

            // Sometimes 2 paths might share a pixel, so we iterate twice to check for all paths
            // the efficency cost should be very small
            // (I'm not sure if there can be 3 paths, seems unlikely given the skeletonization algorithim)
            // We stop if there's a directly adjacent node to avoid excess paths
            bool adjacent_node = false;
            for (int rx = -1; rx <= 1; rx++)
            for (int ry = -1; ry <= 1; ry++) {
                if ((rx == 0 || ry == 0) && (rx || ry)) {
                    if (skeleton_map[y + ry][x + rx] > 1)
                        adjacent_node = true;
                    add_branch_from_skeleton(skeleton, x + rx, y + ry, skeleton_map[y][x], x, y);
                    add_branch_from_skeleton(skeleton, x + rx, y + ry, skeleton_map[y][x], x, y);
                }
            }
            for (int rx = -1; rx <= 1; rx++)
            for (int ry = -1; ry <= 1; ry++) {
                if (rx && ry) {
                    // Directly adjacent node existed earlier, check if the new location is touching a node
                    if (!adjacent_node && (skeleton_map[y + ry][x] > 1 || skeleton_map[y][x + rx] > 1))
                        continue;
                    add_branch_from_skeleton(skeleton, x + rx, y + ry, skeleton_map[y][x], x, y);
                    add_branch_from_skeleton(skeleton, x + rx, y + ry, skeleton_map[y][x], x, y);
                }
            }
        }
    }
}

/* Trim adjacent nodes, nodes often become grouped in arrangments like
 *    N
 *   NNN
 *    N
 * In which case we take only the center (above, the center node)
 * Also reassigns node ids as it deletes nodes, leaving empty gaps
 * in ids */
void Circuit::trim_adjacent_nodes(const coord_vec &nodes) {
    float avgx, avgy;
    int x2, y2, count = 0, new_node_id = 2;
    std::vector<std::pair<int, int> > node_cluster;
    int visited[YRES][XRES];
    bool x_all_same = true, y_all_same = true;
    CoordStack coords;

    std::fill(&visited[0][0], &visited[YRES][0], 0);

    for (auto &pos : nodes) {
        if (skeleton_map[pos.y][pos.x] <= 1 || visited[pos.y][pos.x]) // Already cleared, skip
            continue;
        if (immutable_nodes[pos.y][pos.x]) { // Node is not allowed to be condensed
            int type = 0;
            // Exception: if diagonal node is touching non-adjacent node of same type
            if (immutable_nodes[pos.y][pos.x] == 2) { // 2 means diagonally adjacent
                for (int rx = -1; rx <= 1; ++rx)
                for (int ry = -1; ry <= 1; ++ry)
                    if ((rx || ry) && immutable_nodes[pos.y + ry][pos.x + rx] == 1) {
                        immutable_nodes[pos.y][pos.x] = 0;
                        skeleton_map[pos.y][pos.x] = 1;
                        goto end;
                    }
            }
            skeleton_map[pos.y][pos.x] = new_node_id;
            visited[pos.y][pos.x] = 1;
            new_node_id++;

            type = TYP(sim->pmap[pos.y][pos.x]);
            if (type == GROUND_TYPE) {
                constrained_nodes[new_node_id - 1] = 0.0;
                goto end;
            }
            if (negative_terminal(type)) {
                // Negative terminals of chips are constrained
                for (int rx = -1; rx <= 1; ++rx)
                for (int ry = -1; ry <= 1; ++ry)
                    if ((rx || ry) && is_chip(TYP(sim->pmap[pos.y + ry][pos.x + rx]))) {
                        constrained_nodes[new_node_id - 1] = sim->parts[ID(sim->pmap[pos.y + ry][pos.x + rx])].pavg[1];
                        goto end;
                    }
            }
            end:;
            continue;
        }
            
        count = 0, avgx = 0.0f, avgy = 0.0f;
        coords.clear();
        node_cluster.clear();
        coords.push(pos.x, pos.y);

        while (coords.getSize()) {
            coords.pop(x2, y2);
            if (!immutable_nodes[y2][x2])
                skeleton_map[y2][x2] = 1;
            node_cluster.push_back(std::make_pair(x2, y2));
            avgx += x2, avgy += y2, count++;

            if (node_cluster[node_cluster.size() - 1].first != node_cluster[0].first)
                x_all_same = false;
            if (node_cluster[node_cluster.size() - 1].second != node_cluster[0].second)
                y_all_same = false;

            for (int rx = -1; rx <= 1; ++rx)
            for (int ry = -1; ry <= 1; ++ry)
                if ((rx || ry) && skeleton_map[y2 + ry][x2 + rx] > 1 && !immutable_nodes[y2 + ry][x2 + rx]) {
                    coords.push(x2 + rx, y2 + ry);
                    skeleton_map[y2 + ry][x2 + rx] = 1;
                }
        }
        x2 = (int)(avgx / count + 0.5f), y2 = (int)(avgy / count + 0.5f);
        if (node_cluster.size() == 3 && (x_all_same || y_all_same)) {
            // Special case with a 1px branch, avoid taking center pixel
            // Ie, NNN -> N-N instead of -N- as center picking would result in
            for (auto p : node_cluster) {
                if (p.first == x2 && p.second == y2) // Skip center instead
                    continue;
                skeleton_map[p.second][p.first] = new_node_id;
                visited[p.second][p.first] = 1;
                new_node_id++;
            }
        }
        else if (node_cluster.size() > 5) {
            // Large clusters (>5, which is size of star:)
            //  N    Might indicate a mesh of 1px branches, in which case
            // NNN   we only consider directly adjacent counts
            //  N
            for (auto p : node_cluster) {
                x2 = p.first, y2 = p.second;
                int count = 0;

                for (int rx = -1; rx <= 1; ++rx)
                for (int ry = -1; ry <= 1; ++ry)
                    if ((rx || ry) && (!rx || !ry) && skeleton_map[y2 + ry][x2 + rx])
                        count++;
                if (count > 2) {
                    skeleton_map[p.second][p.first] = new_node_id;
                    visited[p.second][p.first] = 1;
                    new_node_id++;
                }
            }
        }
        else if (skeleton_map[y2][x2] == 1) {
            skeleton_map[y2][x2] = new_node_id;
            visited[y2][x2] = 1;
            new_node_id++;
        }
        else { // Find closest node in cluster
            float closest = -1.0f, distance;
            auto closest_p = node_cluster[0];
            for (auto p : node_cluster) {
                distance = abs(x2 - p.first) + fabs(y2 - p.second);
                if (closest < 0 || distance < closest)
                    closest = distance, closest_p = p;
            }
            skeleton_map[closest_p.second][closest_p.first] = new_node_id;
            visited[closest_p.second][closest_p.first] = 1;
            new_node_id++;
        }
    }
    highest_node_id = new_node_id - 1;
}

void Circuit::add_branch_from_skeleton(const coord_vec &skeleton, int x, int y, int start_node, int sx, int sy) {
    if (!skeleton_map[y][x] || (x == sx && y == sy)) return;

    std::vector<int> ids, rspk_ids, switches, dynamic_resistors;
    double total_resistance = 0.0;
    double total_voltage = 0.0;
    double current_voltage = 0.0;
    double current_gain = 0.0f;
    int source_count = 0;

    int end_node = -1, r, px = sx, py = sy, ox = x, oy = y;
    int diode_type = 0;
    bool found_next;
    char current_polarity = 0; // 0, 1 = positive, -1 = negative

    // Initial polarity
    if (positive_terminal(TYP(sim->pmap[sy][sx]))) current_polarity = 1;
    else if (negative_terminal(TYP(sim->pmap[sy][sx]))) current_polarity = -1;

    // Keep flood filling until we find another node
	while (true) {
        r = sim->pmap[y][x];
        if (is_dynamic_particle(TYP(r)))
            contains_dynamic = true;
        if (is_integration_particle(TYP(r)))
            requires_divergence_checking = true;

        // Polarity handling for voltage drops
        if (positive_terminal(TYP(r))) {
            if (current_polarity == -1)
                total_voltage += current_voltage / std::max(1, source_count);
            current_voltage = 0.0f;
            source_count = 0;
            current_polarity = 1;
            if (TYP(r) == PT_PSCN && TYP(sim->pmap[py][px]) == PT_NSCN)
                diode_type = -1;
        }
        else if (negative_terminal(TYP(r))) {
            if (current_polarity == 1)
                total_voltage += current_voltage / std::max(1, source_count);
            current_voltage = 0.0f;
            current_polarity = -1;
            source_count = 0;
            if (TYP(r) == PT_NSCN && TYP(sim->pmap[py][px]) == PT_PSCN)
                diode_type = 1;
        }
        else if (!is_voltage_source(TYP(r)))
            current_polarity = 0, current_voltage = 0.0f, source_count = 0;

        // Node handling
		if (skeleton_map[y][x] > 1 && skeleton_map[y][x] != start_node) { // Found end_node
            end_node = skeleton_map[y][x];
            total_resistance += get_resistance(TYP(r), sim->parts, ID(r), sim);
            r = sim->pmap[sy][sx];
            total_resistance += get_resistance(TYP(r), sim->parts, ID(r), sim);
            if (skeleton_map[py][px] == 0)
                skeleton_map[py][px] = 1; // Make sure points around nodes are never deleted
            break;
        }
        if (skeleton_map[y][x] == 1) { // Non-node
            skeleton_map[y][x] = 0;
            total_resistance += get_resistance(TYP(r), sim->parts, ID(r), sim);

            ids.push_back(ID(r));
            rspk_ids.push_back(ID(sim->photons[y][x]));
            if (TYP(r) == PT_SWCH)
                switches.push_back(ID(r));
            else if (TYP(r) == PT_INDC)
                current_gain = sim->parts[ID(r)].pavg[1];
            else if (dynamic_resistor(TYP(r)))
                dynamic_resistors.push_back(ID(r));
            else if (is_voltage_source(TYP(r))) {
                if (TYP(r) == PT_VOLT) // Pavg0 is voltage 
                    current_voltage += sim->parts[ID(r)].pavg[0] * current_polarity;
                else if (TYP(r) == PT_CAPR || TYP(r) == PT_INDC) // Pavg1 is "effective" voltage
                    current_voltage += sim->parts[ID(r)].pavg[1] * current_polarity;
                source_count++;
            }
        }
    
        found_next = false;
        px = x, py = y;
        for (size_t ignore_startnode_distance = 0; ignore_startnode_distance <= 1; ignore_startnode_distance++) {
            // Check directly adjacent nodes (Highest priority)
            for (int rx = -1; rx <= 1; ++rx)
            for (int ry = -1; ry <= 1; ++ry)
                if ((rx == 0 || ry == 0) && (rx || ry) && skeleton_map[y + ry][x + rx] > 1 && skeleton_map[y + ry][x + rx] != start_node) {
                    x += rx, y += ry, found_next = true;
                    goto end;
                }
            // Check directly adjacent
            for (int rx = -1; rx <= 1; ++rx)
            for (int ry = -1; ry <= 1; ++ry)
                if ((rx == 0 || ry == 0) && (rx || ry) && skeleton_map[y + ry][x + rx] && skeleton_map[y + ry][x + rx] != start_node &&
                        (ignore_startnode_distance || abs(x + rx - sx) > 1 || abs(y + ry - sy) > 1)) {
                    x += rx, y += ry, found_next = true;
                    goto end;
                }
            // Check for nodes (3rd highest priority)
            // Node: any nodes found below this step cannot be next to start node
            for (int rx = -1; rx <= 1; ++rx)
            for (int ry = -1; ry <= 1; ++ry)
                if ((rx || ry) && skeleton_map[y + ry][x + rx] != start_node && skeleton_map[y + ry][x + rx] > 1 &&
                        (ignore_startnode_distance || abs(x + rx - sx) > 1 || abs(y + ry - sy) > 1)) {
                    x += rx, y += ry, found_next = true;
                    goto end;
                }
            // Check non-adjacent
            for (int rx = -1; rx <= 1; ++rx)
            for (int ry = -1; ry <= 1; ++ry)
                if ((rx && ry) && skeleton_map[y + ry][x + rx] && skeleton_map[y + ry][x + rx] != start_node &&
                        (ignore_startnode_distance || abs(x + rx - sx) > 1 || abs(y + ry - sy) > 1)) {
                    x += rx, y += ry, found_next = true;
                    goto end;
                }
        }
        end:;
        if (!found_next) break; // No end node found, terminate
    }

    total_voltage *= -1; // We incremented voltage drops so far, reverse for voltage gain
    int start_id = ID(sim->photons[sy][sx]);
    int end_id = ID(sim->photons[y][x]);

    if (start_node > end_node) { // Convention: smaller node goes first
        std::swap(start_node, end_node);
        std::swap(start_id, end_id);
        total_voltage *= -1;
        std::reverse(rspk_ids.begin(), rspk_ids.end()); 
    }
    if (skeleton_map[oy][ox] == 0)
        skeleton_map[oy][ox] = 1; // Make sure points around nodes are never deleted

    if (start_node > -1) { // Valid connection actually exists
        Branch * b = new Branch(start_node, end_node, ids, rspk_ids, switches, dynamic_resistors,
            total_resistance, total_voltage, current_gain, diode_type, start_id, end_id);

        /**
         * Special case where branch ends at a node directly adjacent to start node, because branches
         * search diagonals this might result in duplicate branches. Same case applies for branches of length up to 2
         * because end and start are overwritten
         */
        if (b->ids.size() <= 2) {
            // Search backwards as chance of match is higher at ends due to searching near
            // physical proximity
            for (int j = branch_map[start_node].size() - 1; j >= 0; j--) {
                if (branch_map[start_node][j]->node1 == std::min(start_node, end_node) &&
                    branch_map[start_node][j]->node2 == std::max(start_node, end_node) &&
                    (branch_map[start_node][j]->ids.size() <= 2 ||
                     branch_map[start_node][j]->ids == b->ids))
                return;
            }
        } 

        b->setSpecialType(ids.size() && sim->parts[ids[0]].type == PT_CAPR,
                          ids.size() && sim->parts[ids[0]].type == PT_INDC,
                          ids.size() && is_chip(sim->parts[ids[0]].type));

        connection_map[start_node].push_back(end_node);
        connection_map[end_node].push_back(start_node);
        
        branch_cache.push_back(b);
        branch_map[start_node].push_back(b);
        branch_map[end_node].push_back(b);
    }
    // Floating branch
    else if (rspk_ids.size() > 1) {
        Branch * b = new Branch(-1, end_node, ids, rspk_ids, switches, dynamic_resistors,
            total_resistance, total_voltage, current_gain, diode_type, -1, end_id);
        floating_branches[end_node].push_back(b);
        branch_cache.push_back(b);
    }
    // 1 px floating branch may be part of node, reset
    else if (!immutable_nodes[y][x]) {
        skeleton_map[y][x] = 1;
    }
}

/**
 * Solve the circuit using nodal analysis
 */
void Circuit::solve(bool allow_recursion) {
    size_t size = highest_node_id - 1;
    if (!connection_map.size()) return;

    // Don't need to solve: circuit is non-dynamic and we already solved it
    // Force recalc in case something changes like a console command
    if (!contains_dynamic && solution_computed && allow_recursion && sim->timer % FORCE_RECALC_EVERY_N_FRAMES != 0)
        return;

    bool check_divergence = requires_divergence_checking &&
        (!computed_divergence || sim->timer % INTEGRATION_RECALC_EVERY_N_FRAMES == 0);
    if (check_divergence) {
        if (copy) delete copy;
        copy = new Circuit(*this);
        copy->requires_divergence_checking = false;
    }

    // Additional special case solvers
    std::vector<std::pair<int, int> > diode_branches; // Node1 index
    std::vector<std::tuple<int, int, double> > supernodes; // Node 1 Node 2 Voltage
    std::vector<std::pair<int, int> > numeric_integration; // Node 1 index

    // Solve Ax = b
    Eigen::MatrixXd A(size, size);
    Eigen::VectorXd b(size), x(size);
    int j = 0;

    for (size_t row = 0; row < size; row++) {
        auto node_id = connection_map.find(row + 2);
        bool is_constrained;
        double * matrix_row = new double[size + 1];
        std::fill(&matrix_row[0], &matrix_row[size + 1], 0);
        
        // Due to limitations of node finding sometimes false nodes appear, nodes that don't connect
        // to any other nodes at all. We just assign them to ground since they don't do anything
        if (node_id == connection_map.end()) {
            matrix_row[row] = 1;
            goto assign_row;
        }

        is_constrained = constrained_nodes.count(node_id->first);
        
        for (size_t i = 0; i < node_id->second.size(); i++) {
            Branch * b = branch_map[node_id->first][i];

            // Verify diodes and switches
            if (b->isDiode())
                diode_branches.push_back(std::make_pair(node_id->first, i));
            b->computeDynamicResistances(sim, this);
            b->computeDynamicCurrents(sim, this);
            b->computeDynamicVoltages(sim, this);

            if (check_divergence && (b->isInductor() || b->isCapacitor())) {
                numeric_integration.push_back(std::make_pair(node_id->first, i));
                copy->branch_map[node_id->first][i]->setToSteadyState();
            }

            if (!is_constrained) {
                /* Deal with supernodes later */
                if (b->isVoltageSource()) {
                    if (node_id->first < node_id->second[i]) // Avoid duplicate supernodes
                        supernodes.push_back(std::make_tuple(node_id->first, node_id->second[i], b->voltage_gain));
                }
                /* Instead of doing I = (V2 - V1) / R if a branch has a current source
                 * add / subtract current value from end */
                else if (b->isInductor()) {
                    matrix_row[size] = b->current_gain * (node_id->first < node_id->second[i] ? 1 : -1);
                }
                /** Sum of all (N2 - N1) / R = 0 (Ignore resistances across voltage sources, dealt with
                 *  when solving supernodes) */
                else if (b->resistance) {
                    matrix_row[node_id->first - 2] -= 1.0f / b->resistance;
                    matrix_row[node_id->second[i] - 2] += 1.0f / b->resistance;
                }
            }
            if (is_constrained) {
                matrix_row[node_id->first - 2] = 1;
                matrix_row[size] = constrained_nodes[node_id->first];
            }
        }

        assign_row:;
        for (size_t i = 0; i < size; i++)
            A(j, i) = matrix_row[i];
        b[j] = matrix_row[size];
        delete[] matrix_row;
        j++;
    }

    /** Handle supernodes */
    for (size_t j = 0; j < supernodes.size(); j++) {
        int row1 = std::get<0>(supernodes[j]) - 2,
            row2 = std::get<1>(supernodes[j]) - 2;

        A.row(row1) += A.row(row2); // KCL over both end nodes
        for (size_t k = 0; k < size; k++) { // KVL equation
            if ((int)k == row1)      A(row2, k) = -1;
            else if ((int)k == row2) A(row2, k) =  1;
            else                     A(row2, k) =  0;
        }
        b[row2] = std::get<2>(supernodes[j]);
    }

    x = A.colPivHouseholderQr().solve(b);
    
    // std::cout << x << "\n";
    // std::cout << A << "\n";
    // std::cout << b << "\n";
    // std::cout << "\n\n";

    // Diode branches may involve re-solving if diode blocks current or voltage drop is insufficent
    // Same goes for transistors, which can be detected as 2 diodes sharing a node
    bool re_solve = false;
    if (allow_recursion) {
        re_solve = diode_branches.size() > 0; // Diodes force resolve

        // Increase resistance of invalid diodes
        for (size_t i = 0; i < diode_branches.size(); i++) {
            int node1 = diode_branches[i].first;
            int index = diode_branches[i].second;
            int node2 = connection_map[node1][index];
            double deltaV = x[node2 - 2] - x[node1 - 2];

            if (node1 > node2)
                deltaV *= -1; // Maintain polarity: node1 -> node2 is positive
                
            Branch * b = branch_map[node1][index];

            // Fail on the following conditions:
            // 1. Current is flowing right way, but does not meet threshold voltage
            // 2. Current is flowing wrong way and does not exceed breakdown voltage
            // (If deltaV < 0, means current flows from node1 --> node2)
            if ((deltaV < 0 && b->diode > 0 && fabs(deltaV) < DIODE_V_THRESHOLD) || // Correct dir
                (deltaV > 0 && b->diode > 0 && fabs(deltaV) < DIODE_V_BREAKDOWN) || // Wrong dir
                (deltaV > 0 && b->diode < 0 && fabs(deltaV) < DIODE_V_THRESHOLD) || // Correct dir
                (deltaV < 0 && b->diode < 0 && fabs(deltaV) < DIODE_V_BREAKDOWN)    // Wrong dir
            ) {
                b->resistance += REALLY_BIG_RESISTANCE;
            }
            else if (fabs(deltaV) > DIODE_V_THRESHOLD) {
                // Voltage drop across diodes :D
                b->voltage_gain = -DIODE_V_THRESHOLD;
            }
        }
        if (re_solve) solve(false); // Re-solve without diode branches
    }

    // If we don't need to resolve everything, assign currents to each branch
    if (!re_solve) {
        // Used to track current of adjacent branches for branches
        // that do not obey ohms law. We take the current of the ohmian
        // branch going into the node (should only be 1)

        // Order of nodes doesn't matter, we find all branches that share the first node, as 
        // non-ohmian branches must be connected on both ends to be properly connected, ie
        // a voltage source must be arranged as such: [SOME OTHER NODE] -- [NODE1] - V - [NODE2] -- [SOME OTHER NODE]
        // both node1 and node2 must connect to another node, so there will always be a non-ohmian branch to borrow a
        // valid current from
        std::vector<Branch *> non_ohmian_branches;
        double current_of_adjacent = 0.0;
        int node1_of_adjacent = 1;

        for (auto node_id = connection_map.begin(); node_id != connection_map.end(); node_id++) {
            // Normal branches
            non_ohmian_branches.clear();
            for (size_t i = 0; i < node_id->second.size(); i++) {
                Branch * b = branch_map[node_id->first][i];
                b->V1 = x[b->node1 - 2];
                b->V2 = x[b->node2 - 2];

                if (b->obeysOhmsLaw() && b->resistance) {
                    b->current = (b->V1 - b->V2) / b->resistance;
                    current_of_adjacent = b->current;
                    node1_of_adjacent = b->node1;
                }
                else if (!b->obeysOhmsLaw())
                    non_ohmian_branches.push_back(b);
            }
            // Branches that do not obey ohms law take current of adjacent branches
            for (auto &b : non_ohmian_branches) {
                b->current = current_of_adjacent;
                if (node1_of_adjacent == b->node1) // Keep alignment of currents correct 
                    b->current *= -1;
            }

            // Floating branches
            for (size_t i = 0; i < floating_branches[node_id->first].size(); i++)
                floating_branches[node_id->first][i]->V2 = x[floating_branches[node_id->first][i]->node2 - 2];
        }
    }

    // Divergence check complete, set steady state current and voltage
    if (check_divergence) {
        copy->solve();
        for (auto &p : numeric_integration) {
            Branch * b1 = branch_map[p.first][p.second];
            Branch * b2 = copy->branch_map[p.first][p.second];
            b1->SS_voltage = b2->V2 - b2->V1;
            b1->SS_current = b2->current;
        }
    }
    computed_divergence = true;
    solution_computed = true;
}

void Circuit::update_sim() {
    for (auto node_id = connection_map.begin(); node_id != connection_map.end(); node_id++) {
        // Normal branches
        for (size_t i = 0; i < node_id->second.size(); i++) {
            if (node_id->first > node_id->second[i]) continue;

            Branch * b = branch_map[node_id->first][i];
            int prev_type = -1;
            int x = (int)(0.5f + sim->parts[b->node1_id].x),
                y = (int)(0.5f + sim->parts[b->node1_id].y),
                r = sim->pmap[y][x];
            double voltage_drop = 0.0f;

            // Set voltage and current at nodes
            sim->parts[b->node1_id].pavg[0] = restrict_double_to_flt(b->V1);
            sim->parts[b->node1_id].pavg[1] = restrict_double_to_flt(b->current);
            sim->parts[b->node2_id].pavg[0] = restrict_double_to_flt(b->V2);
            sim->parts[b->node2_id].pavg[1] = restrict_double_to_flt(b->current);

            for (auto id : b->rspk_ids) {
                x = (int)(0.5f + sim->parts[id].x);
                y = (int)(0.5f + sim->parts[id].y);
                r = sim->pmap[y][x];

                // Circuit is invalid - just in case somehow the particle
                // below the RSPK was deleted and the RSPK didn't register
                if (!r) {
                    flag_recalc();
                    continue;
                }

                // Consecutive SWCH - don't add resistance otherwise there will be voltage drops
                // (We wish voltage to be uniform across SWCH)
                if (b->obeysOhmsLaw() && !(prev_type == PT_SWCH && TYP(r) == PT_SWCH))
                    voltage_drop += get_effective_resistance(TYP(r), sim->parts, ID(r), sim) * b->current;

                sim->parts[id].pavg[0] = restrict_double_to_flt(b->V1 - voltage_drop);
                sim->parts[id].pavg[1] = restrict_double_to_flt(b->current);

                // Post updates:
                if (TYP(r) == PT_CAPR || TYP(r) == PT_INDC) {
                    double step;
                    // Assign voltages for capcaitor: i / C = dV / dt
                    if (TYP(r) == PT_CAPR) {
                        step = INTEGRATION_TIMESTEP / sim->parts[ID(r)].pavg[0] * b->current;
                        if (fabs(sim->parts[ID(r)].pavg[1] - step) > fabs(b->SS_voltage) &&
                                b->SS_voltage != 0 &&
                                (b->SS_voltage != std::numeric_limits<double>::max() || b->SS_current != std::numeric_limits<double>::max()))
                            sim->parts[ID(r)].pavg[1] = b->SS_voltage;
                        else if (fabs(sim->parts[ID(r)].pavg[1] - b->SS_voltage) < WITHIN_STEADY_STATE)
                            sim->parts[ID(r)].pavg[1] = b->SS_voltage;
                        else
                            sim->parts[ID(r)].pavg[1] -= step;
                    }
                    // Assign current for inductor: V / L =  dI/dt
                    else if (TYP(r) == PT_INDC) {
                        step = (b->V2 - b->V1) / sim->parts[ID(r)].pavg[0] * INTEGRATION_TIMESTEP;
                        if (fabs(sim->parts[ID(r)].pavg[1] - step) > fabs(b->SS_current) &&
                                b->SS_current != 0 &&
                                (b->SS_voltage != std::numeric_limits<double>::max() || b->SS_current != std::numeric_limits<double>::max()))
                            sim->parts[ID(r)].pavg[1] = b->SS_current;
                        else if (fabs(sim->parts[ID(r)].pavg[1] - b->SS_current) < WITHIN_STEADY_STATE)
                            sim->parts[ID(r)].pavg[1] = b->SS_current;
                        else
                            sim->parts[ID(r)].pavg[1] -= step;
                    }
                }
                prev_type = TYP(r);
            }
        }
        // Floating branches
        for (size_t i = 0; i < floating_branches[node_id->first].size(); i++) {
            for (size_t j = 0; j < floating_branches[node_id->first][i]->rspk_ids.size(); j++) {
                int id = floating_branches[node_id->first][i]->rspk_ids[j];
                sim->parts[id].pavg[0] = restrict_double_to_flt(floating_branches[node_id->first][i]->V2);
                sim->parts[id].pavg[1] = restrict_double_to_flt(0.0f);
            }
        }
    }
    // Make RSPK not die
    for (auto id : global_rspk_ids)
        sim->parts[id].life = BASE_RSPK_LIFE;
}

void Circuit::reset_effective_resistances() {
    for (auto b : branch_cache) {
        b->recompute_switches = true;
        b->resistance = b->base_resistance;
    }
}

void Circuit::debug() {
    std::cout << "Circuit connections:\n";
    for (auto itr = connection_map.begin(); itr != connection_map.end(); itr++) {
        std::cout << itr->first << " : ";
        for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++)
            std::cout << *itr2 << " ";
        std::cout << "\n";
    }
    for (auto b : branch_cache)
        b->print();
}

void Circuit::reset() {
    // This is done in generate(), don't need to do it
    // std::fill(&skeleton_map[0][0], &skeleton_map[YRES][0], 0);
    // std::fill(&immutable_nodes[0][0], &immutable_nodes[YRES][0], 0);
    for (auto id : global_rspk_ids)
        circuit_map[id] = nullptr;
    recalc_next_frame = false;
    global_rspk_ids.clear();
    constrained_nodes.clear();
    branch_map.clear();
    floating_branches.clear();
    connection_map.clear();
    for (auto b : branch_cache)
        delete b;
    branch_cache.clear();

    requires_divergence_checking = false;
    contains_dynamic = false;
    solution_computed = false;
    computed_divergence = false;
    highest_node_id = 0;
}

Circuit::Circuit(int x, int y, Simulation * sim) {
    this->sim = sim;
    startx = x, starty = y;
    generate();
    solve();
    update_sim();

    #ifdef DEBUG
        // debug();
    #endif
}

Circuit::Circuit(const Circuit &other) {
    sim = other.sim;
    std::copy(&other.skeleton_map[0][0], &other.skeleton_map[YRES][0], &skeleton_map[0][0]);
    std::copy(&other.immutable_nodes[0][0], &other.immutable_nodes[YRES][0], &immutable_nodes[0][0]);
    connection_map = other.connection_map;
    constrained_nodes = other.constrained_nodes;
    recalc_next_frame =  other.recalc_next_frame;
    startx = other.startx, starty = other.starty;
    contains_dynamic = other.contains_dynamic;
    requires_divergence_checking = other.requires_divergence_checking;
    highest_node_id = other.highest_node_id;

    for (auto node_id = connection_map.begin(); node_id != connection_map.end(); node_id++) {
        for (size_t i = 0; i < node_id->second.size(); i++) {
            Branch * new_b;
            if ((size_t)node_id->first >= branch_map[node_id->second[i]].size() || !branch_map[node_id->second[i]][node_id->first]) {
                new_b = new Branch(*(other.branch_map.at(node_id->first)[i]));
                branch_cache.push_back(new_b);
            }
            else {
                new_b = branch_map[node_id->second[i]][node_id->first];
            }
            branch_map[node_id->first].push_back(new_b);
        }
        // Floating branches
        for (size_t i = 0; i < floating_branches[node_id->first].size(); i++) {
            Branch * new_b = new Branch(*(other.floating_branches.at(node_id->first)[i]));
            floating_branches[node_id->first].push_back(new_b);
            branch_cache.push_back(new_b);
        }
    }
}

Circuit::~Circuit() {
    if (copy) delete copy;
    for (unsigned i = 0; i < branch_cache.size(); i++)
        delete branch_cache[i];
}

void Branch::print() {
    #ifdef DEBUG
        std::cout << "Branch from " << node1 << " -> " << node2 << " R: " << resistance << " V: " << voltage_gain << 
            "  I source: " << current_gain << "    I: " << current << " | diode: " << diode << " | " <<
            V1 << " to " << V2 << " | Node1, node2 id: " << node1_id << " -> " << node2_id << "\n";
    #endif
}

bool Branch::switchesOn(Simulation * sim) {
    if (!recompute_switches) return switches_on;
    switches_on = true;
    for (size_t j = 0; j < switches.size(); j++)
        if (sim->parts[switches[j]].life != 10) {
            switches_on = false;
            break;
        }
    recompute_switches = false;
    return switches_on;
}

void Branch::setSpecialType(bool is_capacitor, bool is_inductor, bool is_chip) {
    if (diode)             branch_type = DIODE;
    else if (is_capacitor) branch_type = CAPACITOR;
    else if (is_inductor)  branch_type = INDUCTOR;
    else if (is_chip)      branch_type = CHIP;
    else if (voltage_gain) branch_type = VOLTAGE_SOURCE;

    obeys_ohms_law = !(is_capacitor || is_inductor || diode || voltage_gain || is_chip);
}

void Branch::setToSteadyState() {
    branch_type = RESISTOR;
    voltage_gain = 0.0;
    current_gain = 0.0;
    obeys_ohms_law = true;
}

void Branch::computeDynamicResistances(Simulation * sim, Circuit * c) {
    if (!switchesOn(sim)) // Increase effective resistances of switches that are off
        resistance += REALLY_BIG_RESISTANCE * switches.size();
    for (auto id : dynamic_resistors)
        resistance += get_effective_resistance(sim->parts[id].type, sim->parts, id, sim);
}

void Branch::computeDynamicVoltages(Simulation *sim, Circuit * c) {
    if (branch_type == CAPACITOR) {
        voltage_gain = 0.0;
        for (auto id : ids)
            voltage_gain += sim->parts[id].pavg[1];
    }
    else if (branch_type == CHIP) {
        float output_voltage = 0.0;
        if (ids.size())
            output_voltage = sim->parts[ids[0]].pavg[1];
        if (c->constrained_nodes.count(node1))
            c->constrained_nodes[node1] = output_voltage;
        else if (c->constrained_nodes.count(node2))
            c->constrained_nodes[node2] = output_voltage;
    }
}

void Branch::computeDynamicCurrents(Simulation * sim, Circuit * c) {
    if (branch_type == INDUCTOR && ids.size())
        current_gain = sim->parts[ids[0]].pavg[1];
}