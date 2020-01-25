#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_TRUS PT_TRUS 305
Element_TRUS::Element_TRUS()
{
	Identifier = "DEFAULT_PT_TRUS";
	Name = "TRUS";
	Colour = PIXPACK(0x303052);
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
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
	Meltable = 1;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "Metal truss. Allows particles through, great for supporting stress. Melts slowly.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC | PROP_HOT_GLOW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1673.0f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_TRUS::update;
}

//#TPT-Directive ElementHeader Element_TRUS static int update(UPDATE_FUNC_ARGS)
int Element_TRUS::update(UPDATE_FUNC_ARGS) {
	if (parts[i].temp > sim->elements[PT_TRUS].HighTemperature - 200.0f && RNG::Ref().chance(1, 500)) {
		sim->part_change_type(i, x, y, PT_LAVA);
		parts[i].ctype = PT_METL;
		return 1;
	}

	return 0;
}

//#TPT-Directive ElementHeader Element_TRUS static int graphics(GRAPHICS_FUNC_ARGS)
int Element_TRUS::graphics(GRAPHICS_FUNC_ARGS) {
	// No reason for this to be here actually
	return 1;
}

Element_TRUS::~Element_TRUS() {}
