#include "simulation/ElementCommon.h"
#include "simulation/magnetics/magnetics.h"
#include "simulation/circuits/framework.h"

//#TPT-Directive ElementClass Element_VOLT PT_VOLT 258
Element_VOLT::Element_VOLT()
{
	Identifier = "DEFAULT_PT_VOLT";
	Name = "VOLT";
	Colour = PIXPACK(0x826108);
	MenuVisible = 1;
	MenuSection = SC_ELECTROMAG;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 1;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "Voltage battery. Set output voltage with pavg0.";

	Properties = TYPE_SOLID;
	DefaultProperties.pavg[0] = 10.0f;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = ITH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_VOLT::update;
	Graphics = &Element_VOLT::graphics;
}

//#TPT-Directive ElementHeader Element_VOLT static int update(UPDATE_FUNC_ARGS)
int Element_VOLT::update(UPDATE_FUNC_ARGS) {
	if (parts[i].tmp > 0)
		return 0;

	int r, rx, ry;
	for (rx = -1; rx <= 1; rx++)
	for (ry = -1; ry <= 1; ry++) {
		r = pmap[y + ry][x + rx];
		if (r) {
			// Floodfill takes 1.499 ms
			// Thinning takes 266.793 ms
			// TODO: time per iteration

			coord_vec f = floodfill(sim, parts, x + rx, y + ry);
			f = coord_stack_to_skeleton(sim, f);

			for (unsigned int i = 0; i < f.size(); i++)
				parts[ID(sim->photons[f[i].y][f[i].x])].life = 10;

			parts[i].tmp = 1;
			return 0;
		}
	}

	// int r = sim->photons[y][x];

	// if (!r || TYP(r) != PT_RSPK) {
	// 	int ni = sim->create_part(-3, x, y, PT_RSPK);
	// 	parts[ni].tmp = 1;
	// 	parts[ni].life = 9;
	// 	parts[ni].pavg[0] = parts[i].pavg[0];
	// }
	// else if (TYP(r) == PT_RSPK)
	// 	parts[ID(r)].pavg[0] = parts[i].pavg[0];

	return 0;
}

//#TPT-Directive ElementHeader Element_VOLT static int graphics(GRAPHICS_FUNC_ARGS)
int Element_VOLT::graphics(GRAPHICS_FUNC_ARGS) {
	return 1;
}

Element_VOLT::~Element_VOLT() {}