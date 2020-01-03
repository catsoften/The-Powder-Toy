#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_SUGR PT_SUGR 188
Element_SUGR::Element_SUGR()
{
	Identifier = "DEFAULT_PT_SUGR";
	Name = "SUGR";
	Colour = PIXPACK(0xFFFFFF);
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
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 2;
	Hardness = 2;
	Description = "Sugar.";

	Properties = TYPE_PART;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_SUGR::update;
	Graphics = &Element_SUGR::graphics;
}

//#TPT-Directive ElementHeader Element_SUGR static int update(UPDATE_FUNC_ARGS)
int Element_SUGR::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_SUGR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_SUGR::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_SUGR::~Element_SUGR() {}
