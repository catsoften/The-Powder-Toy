#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_GRND PT_GRND 260
Element_GRND::Element_GRND()
{
	Identifier = "DEFAULT_PT_GRND";
	Name = "GRND";
	Colour = PIXPACK(0x8f3535);
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
	Description = "Electrical ground. Absorbs current, setting local voltage to 0.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 3000.0f;
	HighTemperatureTransition = PT_LAVA;

	Update = NULL;
	Graphics = NULL;
}

Element_GRND::~Element_GRND() {}
