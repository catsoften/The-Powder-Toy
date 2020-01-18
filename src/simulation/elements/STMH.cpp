#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_STMH PT_STMH 292
Element_STMH::Element_STMH()
{
	Identifier = "DEFAULT_PT_STMH";
	Name = "STMH";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
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

	Flammable = 10;
	Explosive = 0;
	Meltable = 0;
	Hardness = 15;

	Weight = 100;

	HeatConduct = 104;
	Description = "Stomach. Digests food and water and sends nutrients to flesh.";

	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 200.0f + 273.15f;
	HighTemperatureTransition = PT_FIRE;

	Update = &Element_STMH::update;
	Graphics = &Element_STMH::graphics;
}

//#TPT-Directive ElementHeader Element_STMH static int update(UPDATE_FUNC_ARGS)
int Element_STMH::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_STMH static int graphics(GRAPHICS_FUNC_ARGS)
int Element_STMH::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_STMH::~Element_STMH() {}
