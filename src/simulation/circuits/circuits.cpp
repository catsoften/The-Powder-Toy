#include "simulation/circuits/circuits.h"
#include "simulation/circuits/resistance.h"

#include "simulation/ElementCommon.h"
#include <iostream>

Circuit::Circuit(const coord_vec &skeleton, Simulation * sim) {
    this->sim = sim;

    std::fill(&skeleton_map[0][0], &skeleton_map[YRES][0], 0);
    for (auto &pos : skeleton)
        skeleton_map[pos.y][pos.x] = 1;
    
    generate(skeleton);
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
        }
    }
    trim_adjacent_nodes(nodes);

    // Branch generation
    for (auto &pos : skeleton) {
        x = pos.x, y = pos.y;
        if (skeleton_map[y][x] > 1) {
            sim->parts[ID(sim->photons[y][x])].pavg[1] = skeleton_map[y][x];
            for (int rx = -1; rx <= 1; rx++)
            for (int ry = -1; ry <= 1; ry++) {
                // Sometimes 2 paths might share a pixel, so we iterate twice to check for all paths
                // the efficency cost should be very small
                // (I'm not sure if there can be 3 paths, seems unlikely given the skeletonization algorithim)
                add_branch_from_skeleton(skeleton, x + rx, y + ry, skeleton_map[y][x], x, y);
                add_branch_from_skeleton(skeleton, x + rx, y + ry, skeleton_map[y][x], x, y);
            }
        }
    }
}

/* Trim adjacent nodes, nodes often become grouped in arrangments like
 *    N
 *   NNN
 *    N
 * In which case we take only the center (above, the center node) */
void Circuit::trim_adjacent_nodes(const coord_vec &nodes) {
    float avgx, avgy;
    int x2, y2, count = 0, node_id;
    CoordStack coords;

    for (auto &pos : nodes) {
        if (skeleton_map[pos.y][pos.x] <= 1) // Already cleared, skip
            continue;
            
        count = 0, avgx = 0.0f, avgy = 0.0f;
        node_id = skeleton_map[pos.y][pos.x];
        coords.clear();
        coords.push(pos.x, pos.y);

        while (coords.getSize()) {
            coords.pop(x2, y2);
            skeleton_map[y2][x2] = 1;
            avgx += x2, avgy += y2, count++;

            for (int rx = -1; rx <= 1; ++rx)
            for (int ry = -1; ry <= 1; ++ry)
                if ((rx || ry) && skeleton_map[y2 + ry][x2 + rx] > 1) {
                    coords.push(x2 + rx, y2 + ry);
                    skeleton_map[y2 + ry][x2 + rx] = 1;
                }
        }
        skeleton_map[(int)(avgy / count + 0.5f)][(int)(avgx / count + 0.5f)] = node_id;
    }
}

void Circuit::add_branch_from_skeleton(const coord_vec &skeleton, int x, int y, int start_node, int sx, int sy) {
    if (!skeleton_map[y][x] || (x == sx && y == sy)) return;

    std::vector<int> ids;
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
            ids.push_back(ID(r));
            total_resistance += get_resistance(TYP(r), sim->parts, ID(r), sim);
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

    if (start_node > end_node) // Convention: smaller node goes first
        std::swap(start_node, end_node);
    skeleton_map[oy][ox] = 1; // Make sure points around nodes are never deleted

    // test code
    if (start_node > -1) {
        std::cout << "Branch from " << start_node << " -> " << end_node << "  resistance: " << total_resistance << "\n";
    }
    if (start_node != -1)
        for (auto i : ids) {
            if (! skeleton_map[(int)(sim->parts[i].y + 0.5f)][(int)(sim->parts[i].x + 0.5f)])
                sim->kill_part(i);
        }
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