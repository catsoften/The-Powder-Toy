#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_GRND PT_GRND 260
Element_GRND::Element_GRND()
{
	Identifier = "DEFAULT_PT_GRND";
	Name = "GRND";
	Colour = PIXPACK(0xFFFFFF);
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
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "Electrical ground.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_GRND::update;
	Graphics = &Element_GRND::graphics;
}

//#TPT-Directive ElementHeader Element_GRND static int update(UPDATE_FUNC_ARGS)
int Element_GRND::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_GRND static int graphics(GRAPHICS_FUNC_ARGS)
int Element_GRND::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_GRND::~Element_GRND() {}
