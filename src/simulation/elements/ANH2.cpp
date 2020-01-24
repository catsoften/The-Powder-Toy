#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_ANH2 PT_ANH2 304
Element_ANH2::Element_ANH2()
{
	Identifier = "DEFAULT_PT_ANH2";
	Name = "ANH2";
	Colour = PIXPACK(0xFF0705);
	MenuVisible = 1;
	MenuSection = SC_GAS;
	Enabled = 1;

	Advection = 2.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.10f;
	Gravity = 0.00f;
	Diffusion = 3.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 1;

	HeatConduct = 251;
	Description = "Anti Hydrogen. Combusts with OXYG to make WATR. Undergoes fusion at high temperature and pressure.";

	Properties = TYPE_GAS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_ANH2::update;
	Graphics = &Element_ANH2::graphics;
}

//#TPT-Directive ElementHeader Element_ANH2 static int update(UPDATE_FUNC_ARGS)
int Element_ANH2::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_ANH2 static int graphics(GRAPHICS_FUNC_ARGS)
int Element_ANH2::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_ANH2::~Element_ANH2() {}
