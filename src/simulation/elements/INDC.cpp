#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_INDC PT_INDC 264
Element_INDC::Element_INDC()
{
	Identifier = "DEFAULT_PT_INDC";
	Name = "INDC";
	Colour = PIXPACK(0xDF6742);
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
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 1;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "Inductor. Resists sudden changes in current. pavg0 = inductance.";
	DefaultProperties.pavg[0] = 10.0f;
	DefaultProperties.pavg[1] = 0.0f;

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1085.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_INDC::update;
	Graphics = NULL;
}

//#TPT-Directive ElementHeader Element_INDC static int update(UPDATE_FUNC_ARGS)
int Element_INDC::update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * pavg0 - inductance
	 * pavg1 - effective resistance
	 */

	return 0;
}

Element_INDC::~Element_INDC() {}