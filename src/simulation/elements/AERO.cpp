#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_AERO PT_AERO 266
Element_AERO::Element_AERO()
{
	Identifier = "DEFAULT_PT_AERO";
	Name = "AERO";
	Colour = PIXPACK(0x495459);
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
	Meltable = 0;
	Hardness = 10;

	Weight = 100;

	HeatConduct = 0;
	Description = "Aerogel. Does not conduct heat.";

	Properties = TYPE_SOLID | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 9.0f;
	HighPressureTransition = PT_NONE;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_AERO::update;
	Graphics = &Element_AERO::graphics;
}

//#TPT-Directive ElementHeader Element_AERO static int update(UPDATE_FUNC_ARGS)
int Element_AERO::update(UPDATE_FUNC_ARGS) {
	return 1;
}

//#TPT-Directive ElementHeader Element_AERO static int graphics(GRAPHICS_FUNC_ARGS)
int Element_AERO::graphics(GRAPHICS_FUNC_ARGS) {
	return 1;
}

Element_AERO::~Element_AERO() {}
