#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_PAPR PT_PAPR 271
Element_PAPR::Element_PAPR()
{
	Identifier = "DEFAULT_PT_PAPR";
	Name = "PAPR";
	Colour = PIXPACK(0xFAFAFA);
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
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 50;
	Explosive = 0;
	Meltable = 0;
	Hardness = 50;

	Weight = 100;

	HeatConduct = 164;
	Description = "Paper. Can be stained, dissolves in water, burns quickly.";

	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 232.8f + 273.15f; // 451 F
	HighTemperatureTransition = PT_FIRE;

	Update = &Element_PAPR::update;
	Graphics = &Element_PAPR::graphics;
}

//#TPT-Directive ElementHeader Element_PAPR static int update(UPDATE_FUNC_ARGS)
int Element_PAPR::update(UPDATE_FUNC_ARGS) {
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_PAPR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_PAPR::graphics(GRAPHICS_FUNC_ARGS) {
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_PAPR::~Element_PAPR() {}
