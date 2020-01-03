#include "simulation/ElementCommon.h"
#include "simulation/Spaceship.h"
#include <algorithm>
#include <unordered_map>
#include <vector>

/**
Properties:
	pavg[0] - Storing ship id in componenets connected to a COTR
*/

namespace SHIPS {
	
	std::unordered_map<int, std::vector<int>> ships;
	std::array<int, NUMSHIPCOMPONENTS> components{ PT_COTR, PT_HULL, PT_THRS };

	void createShip(int id) {
		ships[id] =  {};
	}

	void translate(Simulation* sim, int id) {
		int rx = sim->parts[id].pavg[0];
		int ry = sim->parts[id].pavg[1];
		sim->parts[id].x += rx;
		sim->parts[id].y += ry;
		for (auto itr = ships[id].begin(); itr != ships[id].end(); ++itr) {
			sim->parts[*itr].x += rx;
			sim->parts[*itr].y += ry;
		}
	}

	bool isComponent(int typ) {
		for (unsigned int i = 0; i < NUMSHIPCOMPONENTS; ++i)
			if (components[i] == typ) return true;
		return false;
	}

	bool isShip(int id) {
		return ships.count(id) > 0;
	}

	void clonePAVG(Simulation* sim, int i, int x, int y) {
		int r, rx, ry;
		for (rx = -1; rx <= 1; rx++)
			for (ry = -1; ry <= 1; ry++)
				if (BOUNDS_CHECK && (rx || ry)) { // Scan around for other components
					r = sim->pmap[y + ry][x + rx];
					if (!r || sim->parts[ID(r)].pavg[0] < 0) continue; // Skip check if doesn't exist or other component doesn't have an id either
					if (isShip(sim->parts[ID(r)].pavg[0]) || isShip(ID(r))) { // If ship id is valid then we can add it to this component
						// Checks if component adjacent is a component type or if it's pointer is
						if (isComponent(sim->parts[ID(r)].type) || isComponent(TYP(r))) {
							sim->parts[i].pavg[0] = sim->parts[ID(r)].pavg[0];
							if (TYP(r) == PT_COTR)
								sim->parts[i].pavg[0] = ID(r);
							ships[sim->parts[i].pavg[0]].push_back(i); // Add component to ship's component vector
							return;
						}
					}

				}
	}
}