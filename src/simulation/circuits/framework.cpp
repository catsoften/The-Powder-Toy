#include "simulation/circuits/framework.h"
#include "simulation/CoordStack.h"

#include <vector>
#include <unordered_set>

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

/*
 * Floodfill touching conductors with RSPRK
 */
coord_vec floodfill(Simulation *sim, Particle *parts, int x, int y) {
    CoordStack coords;
    coord_vec output;
    int id;
    pos temp;
 
    coords.push(x, y);

	while (coords.getSize()) {
		coords.pop(x, y);

		if (!sim->photons[y][x])
			sim->create_part(-3, x, y, PT_RSPK);

		id = ID(sim->photons[y][x]);
        parts[id].tmp2 = 1; // Mark as "visited" // TODO use binary bits?
        parts[id].life = 1000;

        temp.x = x;
        temp.y = y;
        output.push_back(temp);

		// Floodfill
		for (int rx = -1; rx <= 1; ++rx)
		for (int ry = -1; ry <= 1; ++ry)
		if ((rx || ry)) {
			// Floodfill if valid spot. If currently on switch don't conduct to PSCN or NSCN
			// or the switch can't be toggled
			// We also make sure rspk can't flow from consecutive components like
			// VOLT, CAPR, etc...
			// to avoid flickering and weird bugs
			// We can't conduct to water unless voltage is high enough to avoid super high
			// voltage drops resulting from low voltages
			// Also we can't conduct from NSCN to PSCN (diodes)
			int fromtype = TYP(sim->pmap[y][x]);
			int totype = TYP(sim->pmap[y + ry][x + rx]);
			if (totype && parts[ID(sim->photons[y + ry][x + rx])].tmp2 == 0) {
				coords.push(x + rx, y + ry);
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
        std::unordered_set<std::pair<int, int>, pair_hash> &output_unordered_set, short iter) {
    std::vector<int> delete_these_indices;
    short x, y, p2, p3, p4, p5, p6, p7, p8, p9;

    for (unsigned int i = 0; i < output.size(); i++) {
        x = output[i].x;
        y = output[i].y;

        // Edge bounds check
        if (x == 0 || x == XRES - 1 || y == 0 || y == YRES - 1)
            continue;

        // Thinning algorithim
        // Get surrounding points, clockwise starting from point above
        p2 = output_unordered_set.find(std::make_pair(y - 1, x))     != output_unordered_set.end() ? 1 : 0;
        p3 = output_unordered_set.find(std::make_pair(y - 1, x + 1)) != output_unordered_set.end() ? 1 : 0;
        p4 = output_unordered_set.find(std::make_pair(y, x + 1))     != output_unordered_set.end() ? 1 : 0;
        p5 = output_unordered_set.find(std::make_pair(y + 1, x + 1)) != output_unordered_set.end() ? 1 : 0;
        p6 = output_unordered_set.find(std::make_pair(y + 1, x))     != output_unordered_set.end() ? 1 : 0;
        p7 = output_unordered_set.find(std::make_pair(y + 1, x - 1)) != output_unordered_set.end() ? 1 : 0;
        p8 = output_unordered_set.find(std::make_pair(y, x - 1))     != output_unordered_set.end() ? 1 : 0;
        p9 = output_unordered_set.find(std::make_pair(y - 1, x - 1)) != output_unordered_set.end() ? 1 : 0;

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
        output_unordered_set.erase(std::make_pair(output[delete_these_indices[i]].y, output[delete_these_indices[i]].x));
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
    std::unordered_set<std::pair<int, int>, pair_hash> output_unordered_set;
    int diff;

    // Pre-fill the hashunordered_set
    for (pos p : floodfill)
        output_unordered_set.insert(std::make_pair(p.y, p.x));

    // Repeatedly thin the image to 1 px until it can no longer be further thined
    do {
        coord_stack_to_skeleton_iteration(sim, output, output_unordered_set, 0);
        coord_stack_to_skeleton_iteration(sim, output, output_unordered_set, 1);

        diff = prev_size - output.size();
        prev_size = output.size();
    } while (diff);
    return output;
}


