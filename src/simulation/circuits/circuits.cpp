#include "simulation/circuits/circuits.h"
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
         * 
         * Nodes cannot be adjacent, so if another node is found, then terminate
         */
        for (int rx = -1; rx <= 1; rx++)
        for (int ry = -1; ry <= 1; ry++)
            if ((rx || ry) && skeleton_map[y + ry][x + rx]) {
                if (skeleton_map[y + ry][x + rx] > 1) { // No adjacent nodes
                    count = 0;
                    goto end;
                }
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
        end:;
        if (count > 2) {
            std::cout << "Assigning node with ID " << node_id << " at " << x << ", " << y << "\n";
            skeleton_map[y][x] = node_id;
            node_id++;
        }
    }

    // Branch generation
    for (auto &pos : skeleton) {
        x = pos.x, y = pos.y;
        if (skeleton_map[y][x] > 1) {
            for (int rx = -1; rx <= 1; rx++)
            for (int ry = -1; ry <= 1; ry++)
                add_branch_from_skeleton(skeleton, x + rx, y + ry, skeleton_map[y][x]);
        }
    }
}

void Circuit::add_branch_from_skeleton(const coord_vec &skeleton, int x, int y, int start_node) {
    if (!skeleton_map[y][x]) return;

    std::vector<int> ids;
    float total_resistance = 0.0f;
    float total_voltage = 0.0f;
    int end_node = -1;
    int x_ = x, y_ = y;

    // Keep flood filling until we find another node
    CoordStack coords;
    coords.push(x, y);

	while (coords.getSize()) {
		coords.pop(x, y);

		if (skeleton_map[y][x] > 1) {
            if (skeleton_map[y][x] == start_node)
                continue;
            end_node = skeleton_map[y][x];
            break;
        }
        skeleton_map[y][x] = 0;
        sim->parts[ID(sim->photons[y][x])].pavg[0] = start_node;

        // Floodfill
		for (int rx = -1; rx <= 1; ++rx)
		for (int ry = -1; ry <= 1; ++ry)
            if ((rx || ry) && skeleton_map[y + ry][x + rx] && (abs(x_ - x) > 1 || abs(y_ - y) > 1 || rx ^ ry)) {
                coords.push(x + rx, y + ry);
                if (skeleton_map[y + ry][x + rx] == 1)
                    skeleton_map[y + ry][x + rx] = 0;
            }
    }

    if (end_node > -1)
        std::cout << "Branch from " << start_node << " -> " << end_node << "\n";
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