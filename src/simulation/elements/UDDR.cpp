#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_UDDR PT_UDDR 290
Element_UDDR::Element_UDDR()
{
	Identifier = "DEFAULT_PT_UDDR";
	Name = "UDDR";
	Colour = PIXPACK(0xffadad);
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
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

	Flammable = 10;
	Explosive = 0;
	Meltable = 0;
	Hardness = 15;

	Weight = 100;

	HeatConduct = 104;
	Description = "Udder. Makes milk when squeezed (pressure). Requires nutrients to make more milk.";

	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 200.0f + 273.15f;
	HighTemperatureTransition = PT_FIRE;

	Update = &Element_UDDR::update;
	Graphics = &Element_UDDR::graphics;
}

//#TPT-Directive ElementHeader Element_UDDR static int update(UPDATE_FUNC_ARGS)
int Element_UDDR::update(UPDATE_FUNC_ARGS) {
	// TEMP CODE
	if (sim->pv[y / CELL][x / CELL] > 1.0f) {
		int rx, ry, r, rt;

		for (rx = -1; rx < 2; ++rx)
		for (ry = -1; ry < 2; ++ry)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = pmap[y + ry][x + rx];
				if (!r) {
					sim->create_part(-1, x + rx, y + ry, PT_MILK);
				}
			}
	}

	return 0;
}

//#TPT-Directive ElementHeader Element_UDDR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_UDDR::graphics(GRAPHICS_FUNC_ARGS) {
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_UDDR::~Element_UDDR() {}
