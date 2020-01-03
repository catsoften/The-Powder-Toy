#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_SUGR PT_SUGR 188
Element_SUGR::Element_SUGR()
{
	Identifier = "DEFAULT_PT_SUGR";
	Name = "SUGR";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
	Enabled = 1;

	Advection = 0.4f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.3f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 20;
	Explosive = 1;
	Meltable = 2;
	Hardness = 2;
	Description = "Sugar. Great food for bacteria.";

	Properties = TYPE_PART;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 273.15f + 186.0f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_SUGR::update;
}

//#TPT-Directive ElementHeader Element_SUGR static int update(UPDATE_FUNC_ARGS)
int Element_SUGR::update(UPDATE_FUNC_ARGS) {
	int r, rx, ry;
	for (rx=-1; rx<2; rx++)
		for (ry=-1; ry<2; ry++)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = pmap[y+ry][x+rx];

				// Dissolve
				if (TYP(r) == PT_WATR || TYP(r) == PT_DSTW) {
					sim->part_change_type(ID(r), parts[ID(r)].x, parts[ID(r)].y, PT_SWTR);
					if (RNG::Ref().chance(1, 2))
						sim->kill_part(i);
					return 0;
				}

				// Grow YEST
				else if (TYP(r) == PT_YEST && RNG::Ref().chance(1, 300)) {
					sim->part_change_type(i, parts[i].x, parts[i].y, PT_YEST);
					return 0;
				}

				// React with acid to form carbon
				else if (TYP(r) == PT_ACID || TYP(r) == PT_CAUS) {
					sim->part_change_type(ID(r), parts[ID(r)].x, parts[ID(r)].y, PT_CRBN);
					if (RNG::Ref().chance(1, 2))
						sim->kill_part(i);
					return 0;
				}
			} 
	return 0;
}

Element_SUGR::~Element_SUGR() {}
