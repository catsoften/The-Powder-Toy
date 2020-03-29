#include "simulation/circuits/framework.h"
#include "simulation/circuits/resistance.h"
#include "simulation/CoordStack.h"
#include <vector>

/*
 * Floodfill touching conductors with RSPRK
 */
coord_vec floodfill(Simulation *sim, Particle *parts, int x, int y) {
    int tmp = sim->pmap[y][x];
    if (!valid_conductor(TYP(tmp), sim, ID(tmp)))
        return coord_vec();

    CoordStack coords;
    coord_vec output;
    pos temp;

    char visited[YRES][XRES];
    std::fill(&visited[0][0], &visited[YRES][0], 0);
 
    coords.push(x, y);
	while (coords.getSize()) {
		coords.pop(x, y);

		if (!sim->photons[y][x]) {
			int j = sim->create_part(-3, x, y, PT_RSPK);
            if (j < 0) break; // Unable to create new particles
        }

        visited[y][x] = 1;
        temp.x = x;
        temp.y = y;
        output.push_back(temp);

		// Floodfill
		for (int rx = -1; rx <= 1; ++rx)
		for (int ry = -1; ry <= 1; ++ry)
		if ((rx || ry)) {
			// Floodfill if valid spot. If currently on switch don't conduct to PSCN or NSCN
			// or the switch can't be toggled
			int fromtype = TYP(sim->pmap[y][x]);
			int totype = TYP(sim->pmap[y + ry][x + rx]);
			if ((fromtype != PT_SWCH || (totype != PT_PSCN && totype != PT_NSCN)) &&
                totype && !visited[y + ry][x + rx] && valid_conductor(totype, sim, ID(sim->pmap[y + ry][x + rx]))) {
				coords.push(x + rx, y + ry);
                visited[y + ry][x + rx] = 1;
            }
		}
	}

    return output;
}

/**
 * Helper for skeletization
 * Taken from https://web.archive.org/web/20160322113207/http://opencv-code.com/quick-tips/implementation-of-thinning-algorithm-in-opencv/
 * (Zhang-Suen algorithm for image thinning)
 */
void coord_stack_to_skeleton_iteration(Simulation *sim, coord_vec &output,
        int (&output_map)[YRES][XRES], short iter) {
    std::vector<int> delete_these_indices;
    short x, y, p2, p3, p4, p5, p6, p7, p8, p9, typ;
    delete_these_indices.reserve(output.size() / 2);

    for (unsigned int i = 0; i < output.size(); i++) {
        x = output[i].x;
        y = output[i].y;

        // Edge bounds check
        if (x == 0 || x == XRES - 1 || y == 0 || y == YRES - 1)
            continue;

        // Essential circuit elements cannot be trimmed if only 1 px remains
        typ = TYP(sim->pmap[y][x]);
        if (typ == PT_VOLT || typ == PT_GRND || typ == PT_CAPR || typ == PT_INDC) {
            for (int rx = -1; rx <= 1; rx++)
            for (int ry = -1; ry <= 1; ry++)
                if ((rx || ry) && TYP(sim->pmap[y + ry][x + rx]) == typ)
                    goto exit_loop;
            continue;
        }
        exit_loop:;

        // Thinning algorithim
        // Get surrounding points, clockwise starting from point above
        p2 = output_map[y-1][x];
        p3 = output_map[y-1][x+1];
        p4 = output_map[y][x+1];
        p5 = output_map[y+1][x+1];
        p6 = output_map[y+1][x];
        p7 = output_map[y+1][x-1];
        p8 = output_map[y][x-1];
        p9 = output_map[y-1][x-1];

        // A(P1) is the number of 0 to 1 transitions in a clockwise direction from P9 back to itself
        // and B(P1) is the number of non-zero neighbors of P1.
        int A  = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) + 
                 (p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) + 
                 (p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
                 (p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
        int B  = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
        int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
        int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

        if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
            delete_these_indices.push_back(i);
    }

    // Delete all indices marked for deletion
    for (int i = delete_these_indices.size() - 1; i >= 0; i--) {
        output_map[output[delete_these_indices[i]].y][output[delete_these_indices[i]].x] = 0;
        output.erase(output.begin() + delete_these_indices[i]);
    }
}

/*
 * Returns a new vector of coordinates that can be considered
 * the middle portion (a 1px wide skeleton)
 */
coord_vec coord_stack_to_skeleton(Simulation *sim, const coord_vec &floodfill) {
    coord_vec output(floodfill);
    size_t prev_size;
    int output_map[YRES][XRES];
    int diff;

    std::fill(&output_map[0][0], &output_map[YRES][0], 0.0f);
    for (pos p : floodfill)
        output_map[p.y][p.x] = 1;

    // Repeatedly thin the image to 1 px until it can no longer be further thined
    do {
        coord_stack_to_skeleton_iteration(sim, output, output_map, 0);
        coord_stack_to_skeleton_iteration(sim, output, output_map, 1);

        diff = prev_size - output.size();
        prev_size = output.size();
    } while (diff);
    return output;
}


