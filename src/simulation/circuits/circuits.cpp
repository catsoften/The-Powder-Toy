#include "simulation/circuits/circuits.h"
#include "simulation/circuits/resistance.h"

#include "eigen/Core"
#include "eigen/Dense"
#include "eigen/SparseCholesky"
#include "simulation/ElementCommon.h"

#include <iomanip>
#include <iostream>

Circuit * circuit_map[NPART];
std::set<Circuit *> all_circuits;

// Functions
void CIRCUITS::addCircuit(int x, int y, Simulation * sim) {
    int r = sim->photons[y][x];
    if (circuit_map[ID(r)])
        return; // Already part of a circuit

    std::cout << "ADDING CIRCUIT\n";
    Circuit * c = new Circuit(x, y, sim);
    all_circuits.insert(c);
}

void CIRCUITS::deleteCircuit(int i) {

}

void CIRCUITS::updateAllCircuits() {
    for (auto c : all_circuits) {
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
Circuit::Circuit(int x, int y, Simulation * sim) {
    this->sim = sim;

    coord_vec skeleton = floodfill(sim, sim->parts, x, y);
    skeleton = coord_stack_to_skeleton(sim, skeleton);

    std::fill(&skeleton_map[0][0], &skeleton_map[YRES][0], 0);
    std::fill(&immutable_nodes[0][0], &immutable_nodes[YRES][0], 0);
    for (auto &pos : skeleton) {
        skeleton_map[pos.y][pos.x] = 1;
        sim->parts[ID(sim->photons[pos.y][pos.x])].tmp = 1;
    }
    
    generate(skeleton);
    solve();
    update_sim();

    #ifdef DEBUG
        debug();
    #endif
}

/**
 * Mark nodes on the skeleton map, and create branches.
 * Nodes are numbered with an ID
 * starting from 2 (0 = nothing, 1 = wire, 2+ = node_id)
 */
void Circuit::generate(const coord_vec &skeleton) {
    // Node marking
    char count;
    short x, y;
    int node_id = 2;
    std::vector<pos> nodes;

    for (auto &pos : skeleton) {
        count = 0;
        x = pos.x, y = pos.y;

        if (skeleton_map[y][x] > 1) // Already marked as node
            continue;

        /**
         * Special junction (node) cases: NSCN or PSCN connected to any of the following:
         *      capacitor, voltage source, inductor
         * Or the node itself is VOID
         */
        if (TYP(sim->pmap[y][x]) == GROUND_TYPE) {
            skeleton_map[y][x] = node_id;
            immutable_nodes[y][x] = 1;
            nodes.push_back(pos);
            node_id++;
        }
        else if (TYP(sim->pmap[y][x]) == PT_PSCN || TYP(sim->pmap[y][x]) == PT_NSCN) {
            for (int rx = -1; rx <= 1; rx++)
            for (int ry = -1; ry <= 1; ry++)
                if (rx || ry) {
                    int t = TYP(sim->pmap[y + ry][x + rx]);
                    if (t == PT_CAPR || t == PT_VOLT) {
                        skeleton_map[y][x] = node_id;
                        immutable_nodes[y][x] = (rx && ry) + 1;
                        nodes.push_back(pos);
                        node_id++;
                        goto end;
                    }
                }
            end:;
        }

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
            // Exception: if diagonal node is touching non-adjacent node of same type
            if (immutable_nodes[pos.y][pos.x] == 2) {
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

            if (TYP(sim->pmap[pos.y][pos.x]) == GROUND_TYPE)
                ground_nodes.insert(new_node_id - 1);
            end:;
            continue;
        }
            
        count = 0, avgx = 0.0f, avgy = 0.0f;
        coords.clear();
        node_cluster.clear();
        coords.push(pos.x, pos.y);

        while (coords.getSize()) {
            coords.pop(x2, y2);
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
}

void Circuit::add_branch_from_skeleton(const coord_vec &skeleton, int x, int y, int start_node, int sx, int sy) {
    if (!skeleton_map[y][x] || (x == sx && y == sy)) return;

    std::vector<int> ids, rspk_ids, switches;
    double total_resistance = 0.0;
    double total_voltage = 0.0;
    double current_voltage = 0.0;

    int end_node = -1, r, px = sx, py = sy, ox = x, oy = y;
    int pdiode = 0, ndiode = 0;
    bool found_next;
    char current_polarity = 0; // 0, 1 = positive, -1 = negative

    // Initial polarity
    if (TYP(sim->pmap[sy][sx]) == PT_PSCN) current_polarity = 1;
    else if (TYP(sim->pmap[sy][sx]) == PT_NSCN) current_polarity = -1;

    // Keep flood filling until we find another node
	while (true) {
        r = sim->pmap[y][x];

        // Polarity handling for voltage drops
        if (TYP(r) == PT_PSCN) {
            if (current_polarity == -1)
                total_voltage += current_voltage;
            current_voltage = 0.0f;
            current_polarity = 1;
            if (TYP(sim->pmap[py][px]) == PT_NSCN)
                ndiode++;
        }
        else if (TYP(r) == PT_NSCN) {
            if (current_polarity == 1)
                total_voltage += current_voltage;
            current_voltage = 0.0f;
            current_polarity = -1;
            if (TYP(sim->pmap[py][px]) == PT_PSCN)
                pdiode++;
        }
        else if (TYP(r) != PT_VOLT && TYP(r) != PT_CAPR)
            current_polarity = 0, current_voltage = 0.0f;

        // Node handling
		if (skeleton_map[y][x] > 1 && skeleton_map[y][x] != start_node) { // Found end_node
            end_node = skeleton_map[y][x];
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
            else if (TYP(r) == PT_VOLT)
                current_voltage += sim->parts[ID(r)].pavg[0] * current_polarity;
        }
        
        // Check directly adjacent nodes (Highest priority)
        found_next = false;
        px = x, py = y;
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
                    (abs(x + rx - sx) > 1 || abs(y + ry - sy) > 1)) {
                x += rx, y += ry, found_next = true;
                goto end;
            }
        // Check for nodes (3rd highest priority)
        // Node: any nodes found below this step cannot be next to start node
        for (int rx = -1; rx <= 1; ++rx)
		for (int ry = -1; ry <= 1; ++ry)
            if ((rx || ry) && skeleton_map[y + ry][x + rx] != start_node && skeleton_map[y + ry][x + rx] > 1 &&
                    (abs(x + rx - sx) > 1 || abs(y + ry - sy) > 1)) {
                x += rx, y += ry, found_next = true;
                goto end;
            }
        // Check non-adjacent
        for (int rx = -1; rx <= 1; ++rx)
		for (int ry = -1; ry <= 1; ++ry)
            if ((rx && ry) && skeleton_map[y + ry][x + rx] && skeleton_map[y + ry][x + rx] != start_node &&
                    (abs(x + rx - sx) > 1 || abs(y + ry - sy) > 1)) {
                x += rx, y += ry, found_next = true;
                goto end;
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
        Branch * b = new Branch(start_node, end_node, ids, rspk_ids, switches, total_resistance, total_voltage, pdiode, ndiode,
            start_id, end_id);

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

        connection_map[start_node].push_back(end_node);
        connection_map[end_node].push_back(start_node);
        
        branch_cache.push_back(b);
        branch_map[start_node].push_back(b);
        branch_map[end_node].push_back(b);
    }
    // Floating branch
    else {
        Branch * b = new Branch(-1, end_node, ids, rspk_ids, switches, total_resistance, total_voltage, pdiode, ndiode, -1, end_id);
        floating_branches[end_node].push_back(b);
    }
}

/**
 * Solve the circuit using nodal analysis
 */
void Circuit::solve(bool allow_recursion) {
    size_t size = connection_map.size();
    if (!size) return;

    // Additional special case solvers
    std::vector<std::pair<int, int> > diode_branches; // Node1 index
    std::vector<std::tuple<int, int, double> > supernodes; // Node 1 Node 2 Voltage
    int * row_lookup = new int[size]; // Index = node - 2, value = row

    // Solve Ax = b
    Eigen::MatrixXd A(size, size);
    Eigen::VectorXd b(size), x(size);
    int j = 0;

    for (auto node_id = connection_map.begin(); node_id != connection_map.end(); node_id++) {
        double * matrix_row = new double[size + 1];
        std::fill(&matrix_row[0], &matrix_row[size + 1], 0);
        
        bool is_grnd = std::find(ground_nodes.begin(), ground_nodes.end(), node_id->first) != ground_nodes.end();

        for (size_t i = 0; i < node_id->second.size(); i++) {
            Branch * b = branch_map[node_id->first][i];
            row_lookup[node_id->first - 2] = j;
            
            // Verify diodes and switches
            if (b->positive_diodes || b->negative_diodes)
                diode_branches.push_back(std::make_pair(node_id->first, i));
            b->computeDynamicResistances(sim);

            if (!is_grnd) {
                /* Deal with supernodes later */
                if (b->voltage_gain) {
                    if (node_id->first < node_id->second[i]) // Avoid duplicate supernodes
                        supernodes.push_back(std::make_tuple(node_id->first, node_id->second[i], b->voltage_gain));
                }
                /** Sum of all (N2 - N1) / R = 0 (Ignore resistances across voltage sources, dealt with
                 *  when solving supernodes) */
                else if (b->resistance) {
                    matrix_row[node_id->first - 2] -= 1.0f / b->resistance;
                    matrix_row[node_id->second[i] - 2] += 1.0f / b->resistance;
                }
            }
            if (is_grnd) matrix_row[node_id->first - 2] = 1; // GRND = 0 V
        }

        for (size_t i = 0; i < size; i++)
            A(j, i) = matrix_row[i];
        b[j] = matrix_row[size];
        delete[] matrix_row;
        j++;
    }

    /** Handle supernodes */
    for (size_t j = 0; j < supernodes.size(); j++) {
        int n1 = std::get<0>(supernodes[j]), n2 = std::get<1>(supernodes[j]);
        int row1 = row_lookup[n1 - 2],
            row2 = row_lookup[n2 - 2];
        
        A.row(row1) += A.row(row2); // KCL over both end nodes
        for (size_t k = 0; k < size; k++) { // KVL equation
            if ((int)k == n1 - 2)      A(row2, k) = -1;
            else if ((int)k == n2 - 2) A(row2, k) =  1;
            else                       A(row2, k) =  0;
        }
        b[row2] = std::get<2>(supernodes[j]);
    }
    delete[] row_lookup;

    x = A.colPivHouseholderQr().solve(b);

    //std::cout << x << "\n";
    //std::cout << A << "\n";
    // std::cout << b << "\n";
    // std::cout << "\n\n";

    // Diode branches may involve re-solving if diode blocks current or voltage drop is insufficent
    bool re_solve = false;
    if (allow_recursion) {
        // Increase resistance of invalid diodes
        for (size_t i = 0; i < diode_branches.size(); i++) {
            int node1 = diode_branches[i].first;
            int index = diode_branches[i].second;
            double deltaV = x[connection_map[node1][index] - 2] - x[node1 - 2];

            if (node1 > connection_map[node1][index])
                deltaV *= -1; // Maintain polarity: node1 -> node2 is positive
            Branch * b = branch_map[node1][index];

            // Fail on the following conditions:
            // 1. Current is flowing right way, but does not meet threshold voltage
            // 2. Current is flowing wrong way and does not exceed breakdown voltage
            // (If deltaV < 0, means current flows from node1 --> node2)
            if ((deltaV < 0 && b->positive_diodes && fabs(deltaV) < DIODE_V_THRESHOLD * b->positive_diodes) || // Correct dir
                (deltaV > 0 && b->positive_diodes && fabs(deltaV) < DIODE_V_BREAKDOWN * b->positive_diodes) || // Wrong dir
                (deltaV > 0 && b->negative_diodes && fabs(deltaV) < DIODE_V_THRESHOLD * b->negative_diodes) || // Correct dir
                (deltaV < 0 && b->negative_diodes && fabs(deltaV) < DIODE_V_BREAKDOWN * b->negative_diodes)    // Wrong dir
            ) {
                // This commented out code makes diodes ideal (INF resistance)
                // branch_map[node1].erase(branch_map[node1].begin() + index);
                // connection_map[node1].erase(connection_map[node1].begin() + index);
                b->resistance += REALLY_BIG_RESISTANCE;
                re_solve = true;
            }
        }
        if (re_solve) solve(false); // Re-solve without diode branches
    }

    // If we don't need to resolve everything, assign currents to each branch
    if (!re_solve) {
        for (auto node_id = connection_map.begin(); node_id != connection_map.end(); node_id++) {
            // Normal branches
            for (size_t i = 0; i < node_id->second.size(); i++) {
                Branch * b = branch_map[node_id->first][i];
                b->V1 = x[b->node1 - 2];
                b->V2 = x[b->node2 - 2];

                if (b->resistance)
                    b->current = (x[b->node1 - 2] - x[b->node2 - 2]) / b->resistance;
                // if (node_id->first < node_id->second[i])
                //    b->print();
            }
            // Floating branches
            for (size_t i = 0; i < floating_branches[node_id->first].size(); i++)
                floating_branches[node_id->first][i]->V2 = x[floating_branches[node_id->first][i]->node2 - 2];
        }
    }
}

void Circuit::update_sim() {
    for (auto node_id = connection_map.begin(); node_id != connection_map.end(); node_id++) {
        // Normal branches
        for (size_t i = 0; i < node_id->second.size(); i++) {
            if (node_id->first > node_id->second[i]) continue;

            Branch * b = branch_map[node_id->first][i];
            double total_resistance = 0.0f;
            int x, y, r, prev_type = -1;

            // Set voltage and current at nodes
            sim->parts[b->node1_id].pavg[0] = b->V1;
            sim->parts[b->node1_id].pavg[1] = b->current;
            sim->parts[b->node2_id].pavg[0] = b->V2;
            sim->parts[b->node2_id].pavg[1] = b->current;

            circuit_map[b->node1_id] = this;
            circuit_map[b->node2_id] = this;

            for (auto id : b->rspk_ids) {
                x = (int)(0.5f + sim->parts[id].x);
                y = (int)(0.5f + sim->parts[id].y);
                r = sim->pmap[y][x];

                // VOLT has high resistance normally, valid VOLT has 0 resistance
                // Consecutive SWCH - don't add resistance otherwise there will be voltage drops
                //      among individual SWCH pixels
                if (TYP(r) != PT_VOLT && !(prev_type == PT_SWCH && TYP(r) == PT_SWCH))
                    total_resistance += get_effective_resistance(TYP(r), sim->parts, ID(r), sim);
                
                sim->parts[id].pavg[0] = (float)(b->V1 - total_resistance * b->current);
                sim->parts[id].pavg[1] = b->current;
                circuit_map[id] = this;
                prev_type = TYP(r);
            }
        }
        // Floating branches
        for (size_t i = 0; i < floating_branches[node_id->first].size(); i++) {
            for (size_t j = 0; j < floating_branches[node_id->first][i]->rspk_ids.size(); j++) {
                sim->parts[floating_branches[node_id->first][i]->rspk_ids[j]].pavg[0] = floating_branches[node_id->first][i]->V2;
                sim->parts[floating_branches[node_id->first][i]->rspk_ids[j]].pavg[1] = 0.0f;
            }
        }
    }
}

void Circuit::reset_effective_resistances() {
    for (auto b : branch_cache) {
        b->recomputeSwitches = true;
        b->resistance = b->base_resistance;
    }
}

void Circuit::debug() {
    // std::cout << "Circuit connections:\n";
    // for (auto itr = connection_map.begin(); itr != connection_map.end(); itr++) {
    //     std::cout << itr->first << " : ";
    //     for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++)
    //         std::cout << *itr2 << " ";
    //     std::cout << "\n";
    // }
}

Circuit::~Circuit() {
    for (unsigned i = 0; i < branch_cache.size(); i++)
        delete branch_cache[i];
}


// float get_power(int x, int y, Simulation *sim)
// {
//     int r = sim->photons[y][x];
//     float voltage = sim->parts[ID(r)].pavg[1];
//     r = sim->pmap[y][x];
//     float resistance = get_resistance(TYP(r), sim->parts, ID(r), sim);
//     if (resistance == 0.0f)
//         return 0.0f;
//     return voltage / resistance * voltage;
// }

// /**
//  * Returns type of ground
//  * 0 = Not a ground
//  * 1 = Ground voltage is 0
//  * 2 = Don't multiply voltage drop
//  */
// int is_ground(Particle *parts, int i)
// {
//     if (!i)
//         return 0;
//     if (parts[i].type == PT_VOID)
//         return 1;
//     else if (parts[i].type == PT_VCMB)
//         return 2;
//     return 0;
// }


void Branch::print() {
    #ifdef DEBUG
        std::cout << "Branch from " << node1 << " -> " << node2 << " R: " << resistance << " V: " << voltage_gain << 
            " I: " << current << " | pdiode: " << positive_diodes << " ndiode: " << negative_diodes << " | " <<
            V1 << " to " << V2 << " | Node1, node2 id: " << node1_id << " -> " << node2_id << "\n";
    #endif
}

bool Branch::switchesOn(Simulation * sim) {
    if (!recomputeSwitches) return switches_on;
    switches_on = true;
    for (size_t j = 0; j < switches.size(); j++)
        if (sim->parts[switches[j]].life != 10) {
            switches_on = false;
            break;
        }
    recomputeSwitches = false;
    return switches_on;
}

void Branch::computeDynamicResistances(Simulation * sim) {
    if (!switchesOn(sim)) // Increase effective resistances of switches that are off
        resistance += REALLY_BIG_RESISTANCE * switches.size();
}