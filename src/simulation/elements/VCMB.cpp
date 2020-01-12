#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_VCMB PT_VCMB 260
Element_VCMB::Element_VCMB()
{
	Identifier = "DEFAULT_PT_VCMB";
	Name = "VCMB";
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
	Description = "Combiner junction. Accepts voltage from PSCN, outputs combined voltage to NSCN.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_VCMB::update;
	Graphics = &Element_VCMB::graphics;
}

//#TPT-Directive ElementHeader Element_VCMB static int update(UPDATE_FUNC_ARGS)
int Element_VCMB::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_VCMB static int graphics(GRAPHICS_FUNC_ARGS)
int Element_VCMB::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_VCMB::~Element_VCMB() {}
