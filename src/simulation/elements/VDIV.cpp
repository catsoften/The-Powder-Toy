#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_VDIV PT_VDIV 259
Element_VDIV::Element_VDIV()
{
	Identifier = "DEFAULT_PT_VDIV";
	Name = "VDIV";
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
	Description = "Divider junction. Accepts single input from PSCN, divides current to NSCN.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_VDIV::update;
	Graphics = &Element_VDIV::graphics;
}

//#TPT-Directive ElementHeader Element_VDIV static int update(UPDATE_FUNC_ARGS)
int Element_VDIV::update(UPDATE_FUNC_ARGS) {
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_VDIV static int graphics(GRAPHICS_FUNC_ARGS)
int Element_VDIV::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_VDIV::~Element_VDIV() {}
