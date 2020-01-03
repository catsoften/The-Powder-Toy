#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_STAR PT_STAR 197
Element_STAR::Element_STAR()
{
	Identifier = "DEFAULT_PT_STAR";
	Name = "STAR";
	Colour = PIXPACK(0xFFFF00);
	MenuVisible = 1;
	MenuSection = SC_SPACE;
	Enabled = 1;

	Advection = 0.00f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.00f;
	Loss = 0.00f;
	Collision = 0.00f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f  * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 100;

	Weight = -1;

	DefaultProperties.temp = R_TEMP + 4.0f + 273.15f;
	
	HeatConduct = 10;
	Description = "Star, undergoes stellar evolution and provides light for planets.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_STAR::update;
	Graphics = &Element_STAR::graphics;
}

//#TPT-Directive ElementHeader Element_STAR static int update(UPDATE_FUNC_ARGS)
int Element_STAR::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_STAR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_STAR::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_STAR::~Element_STAR() {}
