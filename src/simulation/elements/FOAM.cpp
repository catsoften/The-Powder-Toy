#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_FOAM PT_FOAM 310
Element_FOAM::Element_FOAM()
{
	Identifier = "DEFAULT_PT_FOAM";
	Name = "FOAM";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	Advection = 0.7f;
	AirDrag = 0.02f * CFDS;
	AirLoss = 0.96f;
	Loss = 0.80f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 1;

	Flammable = 10;
	Explosive = 0;
	Meltable = 0;
	Hardness = 30;

	Weight = 18;

	HeatConduct = 70;
	Description = "Foam. Foamy.";

	Properties = TYPE_PART | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_FOAM::update;
	Graphics = &Element_FOAM::graphics;
}

//#TPT-Directive ElementHeader Element_FOAM static int update(UPDATE_FUNC_ARGS)
int Element_FOAM::update(UPDATE_FUNC_ARGS) {
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_FOAM static int graphics(GRAPHICS_FUNC_ARGS)
int Element_FOAM::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_FOAM::~Element_FOAM() {}
