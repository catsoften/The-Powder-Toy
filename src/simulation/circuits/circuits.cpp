#include "simulation/circuits/circuits.h"
#include "simulation/circuits/resistance.h"

#include "eigen/Core"
#include "eigen/Dense"
#include "eigen/SparseCholesky"
#include "simulation/ElementCommon.h"

#include <iomanip>
#include <iostream>

Circuit::Circuit(const coord_vec &skeleton, Simulation * sim) {
    this->sim = sim;

    std::fill(&skeleton_map[0][0], &skeleton_map[YRES][0], 0);
    std::fill(&immutable_nodes[0][0], &immutable_nodes[YRES][0], 0);
    for (auto &pos : skeleton)
        skeleton_map[pos.y][pos.x] = 1;
    
    generate(skeleton);
    solve();

    #ifdef DEBUG
        // debug();
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
            sim->parts[ID(sim->photons[y][x])].pavg[1] = skeleton_map[y][x]; // TODO

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

    std::vector<int> ids, switches;
    float total_resistance = 0.0f;
    float total_voltage = 0.0f;
    float current_voltage = 0.0f;

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

    if (start_node > end_node) { // Convention: smaller node goes first
        std::swap(start_node, end_node);
        total_voltage *= -1;
    }
    if (skeleton_map[oy][ox] == 0)
        skeleton_map[oy][ox] = 1; // Make sure points around nodes are never deleted

    if (start_node > -1) { // Valid connection actually exists
        Branch * b = new Branch(start_node, end_node, ids, switches, total_resistance, total_voltage, pdiode, ndiode);

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

    // test code
    if (start_node > -1) {
        std::cout << "Branch from " << start_node << " -> " << end_node << "  resistance: " << total_resistance
            << " total volt: " << total_voltage << "\n";
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

    // Solve Ax = b
    Eigen::MatrixXd A(size, size);
    Eigen::VectorXd b(size), x(size);
    int j = 0;

    for (auto node_id = connection_map.begin(); node_id != connection_map.end(); node_id++) {
        double * matrix_row = new double[size + 1];
        std::fill(&matrix_row[0], &matrix_row[size + 1], 0);
        
        bool is_grnd = std::find(ground_nodes.begin(), ground_nodes.end(), node_id->first) != ground_nodes.end();
        bool branch_has_valid_switches = true;

        for (size_t i = 0; i < node_id->second.size(); i++) {
            Branch * b = branch_map[node_id->first][i];
            
            // Verify diodes and switches
            if (b->positive_diodes || b->negative_diodes)
                diode_branches.push_back(std::make_pair(node_id->first, i));
            
            branch_has_valid_switches = true;
            for (size_t j = 0; j < b->switches.size(); j++)
                if (!sim->parts[b->switches[j]].life) {
                    branch_has_valid_switches = false;
                    break;
                }
            if (!branch_has_valid_switches) // Ignore switches that are "OFF"
                continue;

            if (!is_grnd) {
                /** N1 + V = N2, or N2 - N1 = V */
                if (b->voltage_gain && node_id->first < node_id->second[i]) {
                    std::fill(&matrix_row[0], &matrix_row[size + 1], 0);
                    int m = node_id->first < node_id->second[i] ? 1 : -1;

                    matrix_row[node_id->first - 2] = -1 * m ;
                    matrix_row[node_id->second[i] - 2] = 1 * m;
                    matrix_row[size] = b->voltage_gain;
                    break;
                }
                /** Sum of all (N2 - N1) / R = 0 */
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

    x = A.colPivHouseholderQr().solve(b);

    std::cout << x << "\n";
    std::cout << A << "\n";
    std::cout << b << "\n";
    std::cout << "\n\n";

    // Diode branches may involve re-solving if diode blocks current or voltage drop is insufficent
    if (allow_recursion) {
        bool re_solve = false;

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
}

void Circuit::debug() {
    std::cout << "Circuit connections:\n";
    for (auto itr = connection_map.begin(); itr != connection_map.end(); itr++) {
        std::cout << itr->first << " : ";
        for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++)
            std::cout << *itr2 << " ";
        std::cout << "\n";
    }
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

// bool is_voltage_valid(Simulation *sim, Particle *parts, int x, int y, int counter, float voltage)
// {
//     // New location must have an rspk
//     int r = sim->photons[y][x];
//     int r2 = sim->pmap[y][x];
//     if (TYP(r) != PT_RSPK && !valid_conductor(TYP(r2), sim, ID(r2)))
//         return false;

//     // New rspk location must not have been visited (tmp2 = 1)
//     if (r && parts[ID(r)].tmp2 == 1)
//         return false;
//     return true;
// }