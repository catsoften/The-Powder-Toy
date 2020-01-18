#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_POTO PT_POTO 299
Element_POTO::Element_POTO()
{
	Identifier = "DEFAULT_PT_POTO";
	Name = "POTO";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	Advection = 0.4f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.3f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 1;

	Flammable = 5;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 50;

	HeatConduct = 150;
	Description = "Potatoes.";

	Properties = TYPE_PART;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 273.15 + 600.0f;
	HighTemperatureTransition = PT_FIRE;

	Update = &Element_POTO::update;
	Graphics = &Element_POTO::graphics;
}

//#TPT-Directive ElementHeader Element_POTO static int update(UPDATE_FUNC_ARGS)
int Element_POTO::update(UPDATE_FUNC_ARGS) {
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_POTO static int graphics(GRAPHICS_FUNC_ARGS)
int Element_POTO::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_POTO::~Element_POTO() {}
