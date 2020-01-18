#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_009G PT_009G 298
Element_009G::Element_009G()
{
	Identifier = "DEFAULT_PT_009G";
	Name = "009G";
	Colour = PIXPACK(0xff3636);
	MenuVisible = 0;
	MenuSection = SC_GAS;
	Enabled = 1;

	Advection = 1.0f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.1f;
	Gravity = -0.1f;
	Diffusion = 0.75f;
	HotAir = 0.0003f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 4;

	Weight = 1;

	DefaultProperties.temp = -R_TEMP - 100.0f + 273.15f;
	HeatConduct = 48;
	Description = "Gaseous SCP-009.";

	Properties = TYPE_GAS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 171.0f;
	HighTemperatureTransition = PT_009;

	Update = &Element_009G::update;
}

//#TPT-Directive ElementHeader Element_009G static int update(UPDATE_FUNC_ARGS)
int Element_009G::update(UPDATE_FUNC_ARGS) {
	return Element_009::update(sim, i, x, y, surround_space, nt, parts, pmap);
}

Element_009G::~Element_009G() {}
