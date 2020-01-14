#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_BRAS PT_BRAS 265
Element_BRAS::Element_BRAS()
{
	Identifier = "DEFAULT_PT_BRAS";
	Name = "BRAS";
	Colour = PIXPACK(0xe3c359);
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
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 1;
	Hardness = 20;

	Weight = 100;

	HeatConduct = 251;
	Description = "Brass. Kills bacteria, can be used for thermoelectric generation.";
	PhotonReflectWavelengths = 0xFFA0000;

	Properties = TYPE_SOLID|PROP_CONDUCTS|PROP_LIFE_DEC|PROP_HOT_GLOW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 1.0f;
	HighPressureTransition = ST;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 900.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_BRAS::update;
	Graphics = &Element_BRAS::graphics;
}

//#TPT-Directive ElementHeader Element_BRAS static int update(UPDATE_FUNC_ARGS)
int Element_BRAS::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_BRAS static int graphics(GRAPHICS_FUNC_ARGS)
int Element_BRAS::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_BRAS::~Element_BRAS() {}
