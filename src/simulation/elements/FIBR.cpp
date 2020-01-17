#include "simulation/ElementCommon.h"
#include <vector>

const int MAX_PHOT_SPEED = 30;

//#TPT-Directive ElementClass Element_FIBR PT_FIBR 261
Element_FIBR::Element_FIBR()
{
	Identifier = "DEFAULT_PT_FIBR";
	Name = "FIBR";
	Colour = PIXPACK(0xA9C8AB);
	MenuVisible = 1;
	MenuSection = SC_ELEC;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 100;

	HeatConduct = 150;
	Description = "Fiber optic cable. Moves photons very quickly.";

	Properties = TYPE_SOLID | PROP_HOT_GLOW | PROP_SPARKSETTLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 5.0f;
	HighPressureTransition = PT_BGLA;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_FIBR::update;
	Graphics = &Element_FIBR::graphics;
}

//#TPT-Directive ElementHeader Element_FIBR static int update(UPDATE_FUNC_ARGS)
int Element_FIBR::update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties
	 * - ctype:  PHOT wavelength
	 * 
	 * Increases life of PHOT inside
	 */

	// Melt into glass
	if (parts[i].temp > 1973.15f) {
		sim->part_change_type(i, x, y, PT_LAVA);
		parts[i].ctype = PT_GLAS;
		return 1;
	}

	// Move photons along the cable
	int r = sim->photons[y][x];
	if (TYP(r) != PT_PHOT)
		return 0;

	parts[ID(r)].life++; // Keep phot alive
	int dx = 0, dy = 0;
	int vx = isign(parts[ID(r)].vx), vy = isign(parts[ID(r)].vy);

	// Find furthest point along phot velocity that it can remain inside FIBR
	while (abs(dx) < MAX_PHOT_SPEED && abs(dy) < MAX_PHOT_SPEED) {
		dx += vx, dy += vy;

		// Bounds check and make sure velocity is not 0
		if (x + dx >= 0 && x + dx < XRES && y + dy >= 0 && y + dy < YRES) {
			int r2 = pmap[y + dy][x + dx];
			if (TYP(r2) != PT_FIBR) {
				std::vector<std::pair<int, int> > branches;
				dx -= vx;
				dy -= vy;

				if (abs(dx) == 0 || abs(dy) == 0) {
					// Look for nearby FIBR to redirect to
					for (int rx = -1; rx <= 1; ++rx)
					for (int ry = -1; ry <= 1; ++ry)
						if (BOUNDS_CHECK && (rx || ry)) {
							int r2 = pmap[y + ry][x + rx];
							if (r2 && TYP(r2) == PT_FIBR && (rx != -vx || ry != -vy) && (rx == 0 || ry == 0))
								branches.push_back(std::make_pair(rx, ry));
						}

					// Redirect on single curve
					// TODO randomly pick a path to go to

					if (branches.size() == 0) {
						// Avoid stopping at end
						dx = vx;
						dy = vy;
					}
					if (branches.size() == 1) {
						dx = branches[0].first;
						dy = branches[0].second;
					}
					else if (branches.size() > 1) {
						int j = RNG::Ref().between(0, branches.size() - 1);
						dx = branches[j].first;
						dy = branches[j].second;

						// // parts[ID(r)].ctype = 0; // Distribute energy
						// for (unsigned int j = 1; j < branches.size(); j++) {
						// 	int ni = sim->create_part(-3, x + dx, y + dy, PT_PHOT);
						// 	parts[ni].temp = 0; // Copy temp life and shit from this
						// 	parts[ni].ctype = parts[ID(r)].ctype;
						// 	parts[ni].vx = branches[j].first;
						// 	parts[ni].vy = branches[j].second;
						// }
					}
				}
				break;
			}
		}
		else { // Out of bounds
			dx = vx;
			dy = vy;
			break;
		}
	}
	parts[ID(r)].vx = dx;
	parts[ID(r)].vy = dy;

	return 0;
}

//#TPT-Directive ElementHeader Element_FIBR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_FIBR::graphics(GRAPHICS_FUNC_ARGS) {
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_FIBR::~Element_FIBR() {}
