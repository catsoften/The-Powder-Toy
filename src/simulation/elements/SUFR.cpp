#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_SUFR PT_SUFR 294
Element_SUFR::Element_SUFR()
{
	Identifier = "DEFAULT_PT_SUFR";
	Name = "SUFR";
	Colour = PIXPACK(0xdbd032);
	MenuVisible = 1;
	MenuSection = SC_POWDERS;
	Enabled = 1;

	Advection = 0.4f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.3f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 1;

	Flammable = 50;
	Explosive = 0;
	Meltable = 5;
	Hardness = 0;

	Weight = 60;
	HeatConduct = 150;
	Description = "Sulfur. Burns quickly into a blue flame.";

	Properties = TYPE_PART;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 115.2f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_SUFR::update;
	Graphics = &Element_SUFR::graphics;
}

//#TPT-Directive ElementHeader Element_SUFR static int update(UPDATE_FUNC_ARGS)
int Element_SUFR::update(UPDATE_FUNC_ARGS) {
	int rx, ry, r, rt;

	for (rx = -1; rx < 2; ++rx)
	for (ry = -1; ry < 2; ++ry)
		if (BOUNDS_CHECK && (rx || ry)) {
			r = pmap[y + ry][x + rx];
			if (!r) continue;
			rt = TYP(r);

			// Blue fire
			if (rt == PT_FIRE) {
				parts[ID(r)].dcolour = 0xff80dfff;
				parts[ID(r)].tmp2 = 1;
				parts[ID(r)].temp += 500.0f;
				sim->part_change_type(i, x, y, PT_CAUS);
				return 1;
			}

			else if (rt == PT_GLOW) {
				sim->kill_part(ID(r));
				sim->part_change_type(i, x, y, PT_ACID);
				return 1;
			}
			else if (rt == PT_BCOL) {
				sim->kill_part(ID(r));
				sim->part_change_type(i, x, y, PT_GUNP);
				return 1;
			}
		}

	return 0;
}

//#TPT-Directive ElementHeader Element_SUFR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_SUFR::graphics(GRAPHICS_FUNC_ARGS) {
	return 1;
}

Element_SUFR::~Element_SUFR() {}
