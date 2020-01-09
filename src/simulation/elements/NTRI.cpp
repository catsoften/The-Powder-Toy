#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
void Element_NEUT_create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_NTRI() {
	Identifier = "DEFAULT_PT_NTRI";
	Name = "NTRI";
	Colour = PIXPACK(0xeb34c3);
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = -0.99f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = -1;

	DefaultProperties.temp = R_TEMP + 900.0f + 273.15f;
	HeatConduct = 0;
	Description = "Neutrinos. Goes through everything, interacts with neutrons and electrons.";

	Properties = TYPE_ENERGY|PROP_LIFE_DEC|PROP_LIFE_KILL_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &graphics;
	Create = &Element_NEUT_create;
}

static int update(UPDATE_FUNC_ARGS) {
	int rx, ry, r;
	for (rx = -1; rx <= 1; ++rx)
	for (ry = -1; ry <= 1; ++ry)
		if (BOUNDS_CHECK && (rx || ry)) {
			r = sim->photons[y][x];
			if (!r) continue;

			// NEUT -> ELEC + PHOT
			// ELEC -> PHOT
			if (TYP(r) == PT_NEUT) {
				sim->part_change_type(ID(r), x + rx, y + ry, PT_ELEC);
				int ni = sim->create_part(-3, x + rx, y + ry, PT_PHOT);
				if (ni > -1) parts[ni].temp = parts[i].temp;
			}
			else if (TYP(r) == PT_ELEC) {
				sim->part_change_type(ID(r), x + rx, y + ry, PT_PHOT);
			}
		}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS) {
	// Glow if inside another element
	if (ren->sim->pmap[ny][nx]) {
		*firer = *colr;
		*fireg = *colg;
		*fireb = *colb;
		*firea = 255;
		*pixel_mode |= FIRE_ADD;
	}

	return 0;
}



