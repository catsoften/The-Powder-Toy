#include "simulation/circuits/circuits.h"
#include "simulation/circuits/resistance.h"

#include "simulation/ElementCommon.h"
#include <iostream>

Circuit::Circuit(const coord_vec &skeleton, Simulation * sim) {
    this->sim = sim;

    std::fill(&skeleton_map[0][0], &skeleton_map[YRES][0], 0);
    std::fill(&immutable_nodes[0][0], &immutable_nodes[YRES][0], 0);
    for (auto &pos : skeleton)
        skeleton_map[pos.y][pos.x] = 1;
    
    generate(skeleton);
    // solve();

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
         */
        if (TYP(sim->pmap[y][x]) == PT_PSCN || TYP(sim->pmap[y][x]) == PT_NSCN) {
            for (int rx = -1; rx <= 1; rx++)
            for (int ry = -1; ry <= 1; ry++)
                if (rx || ry) {
                    int t = TYP(sim->pmap[y + ry][x + rx]);
                    if (t == PT_CAPR || t == PT_VOLT || t == PT_INDC) {
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
            for (int rx = -1; rx <= 1; rx++)
            for (int ry = -1; ry <= 1; ry++) {
                // Sometimes 2 paths might share a pixel, so we iterate twice to check for all paths
                // the efficency cost should be very small
                // (I'm not sure if there can be 3 paths, seems unlikely given the skeletonization algorithim)
                //add_branch_from_skeleton(skeleton, x + rx, y + ry, skeleton_map[y][x], x, y);
                //add_branch_from_skeleton(skeleton, x + rx, y + ry, skeleton_map[y][x], x, y);
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
            new_node_id++;
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
    int end_node = -1, r, px, py, ox = x, oy = y;
    bool found_next;

    // Keep flood filling until we find another node
	while (true) {
        r = sim->pmap[y][x];
		if (skeleton_map[y][x] > 1 && skeleton_map[y][x] != start_node) { // Found end_node
            end_node = skeleton_map[y][x];
            total_resistance += get_resistance(TYP(r), sim->parts, ID(r), sim);
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
                total_voltage += sim->parts[ID(r)].pavg[0];
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
    skeleton_map[oy][ox] = 1; // Make sure points around nodes are never deleted

    if (start_node > -1) { // Valid connection actually exists
        connection_map[start_node].push_back(end_node);
        connection_map[end_node].push_back(start_node);

        Branch * b = new Branch(start_node, end_node, ids, switches, total_resistance, total_voltage);
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
void Circuit::solve() {
    for (auto node_id = connection_map.begin(); node_id != connection_map.end(); node_id++) {
        float * matrix_row = new float[connection_map.size()];
        std::fill(&matrix_row[0], &matrix_row[connection_map.size()], 0);

        for (size_t i = 0; i < node_id->second.size(); i++) {
            Branch * b = branch_map[node_id->first][i];
            if (b->resistance) {
                matrix_row[node_id->first - 2] -= 1.0f / b->resistance;
                matrix_row[node_id->second[i] - 2] += 1.0f / b->resistance;
            }
        }
        for (size_t i = 0; i < connection_map.size(); i++)
            std::cout << matrix_row[i] << " ";
        std::cout << "\n";
        delete matrix_row;
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