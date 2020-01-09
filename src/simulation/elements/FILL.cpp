#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_FILL()
{
	Identifier = "DEFAULT_PT_FILL";
	Name = "FILL";
	Colour = PIXPACK(0x172642);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = -1;

	HeatConduct = 0;
	Description = "Filler. Does not interact with most elements, can be used for backgrounds.";

	Properties = TYPE_SOLID | PROP_INDESTRUCTIBLE;

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
	return 1;
}

static int graphics(GRAPHICS_FUNC_ARGS) {
	return 1;
}



