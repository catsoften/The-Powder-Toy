#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_N2() {
	Identifier = "DEFAULT_PT_N2";
	Name = "N2";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 10;
	HeatConduct = 34;
	Description = "DESCRIPTION";

	Properties = TYPE_GAS;

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
}

static int update(UPDATE_FUNC_ARGS) {
	for (int rx = -1; rx <= 1; rx++)
	for (int ry = -1; ry <= 1; ry++)
		if (BOUNDS_CHECK && (rx || ry)) {
			int r = pmap[y + ry][x + rx];
			if (!r) continue;
		}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS) {
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}
