#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_RSTR PT_RSTR 256
Element_RSTR::Element_RSTR() {
	Identifier = "DEFAULT_PT_RSTR";
	Name = "RSTR";
	Colour = PIXPACK(0xEACE9E);
	MenuVisible = 1;
	MenuSection = SC_ELECTROMAG;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.0f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 10;
	Weight = 100;

	HeatConduct = 2;
	Description = "Resistor. Set resistivity / px as pavg0. Will melt if exceeds rated max voltage (pavg1)";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;

	DefaultProperties.pavg[0] = 1000;
	DefaultProperties.pavg[1] = 10;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1400.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_RSTR::update;
	Graphics = NULL;
}

//#TPT-Directive ElementHeader Element_RSTR static int update(UPDATE_FUNC_ARGS)
int Element_RSTR::update(UPDATE_FUNC_ARGS) {
	// pavg0 = restivity
	// pavg1 = max rated voltage
	if (parts[i].pavg[0] <= 0.0f)
		parts[i].pavg[0] = 1.0f;

	// int r = sim->photons[y][x];
	// if (r && TYP(r) == PT_RSPK) {
	// 	float voltage = parts[ID(r)].pavg[1];

	// 	// Nearing max rated voltage, smoke
	// 	if (voltage > parts[i].pavg[1] * 0.95f) {
	// 		if (RNG::Ref().chance(1, 500)) {
	// 			if (RNG::Ref().chance(1, 3))
	// 				sim->create_part(-1, x - 1, y, PT_SMKE);
	// 			if (RNG::Ref().chance(1, 3))
	// 				sim->create_part(-1, x + 1, y, PT_SMKE);
	// 			if (RNG::Ref().chance(1, 3))
	// 				sim->create_part(-1, x, y - 1, PT_SMKE);
	// 			if (RNG::Ref().chance(1, 3))
	// 				sim->create_part(-1, x, y + 1, PT_SMKE);
	// 		}
	// 	}
	// 	// We reached max rated voltage, melt
	// 	if (voltage >= parts[i].pavg[1]) {
	// 		sim->part_change_type(i, x, y, PT_LAVA);
	// 		parts[i].ctype = PT_RSTR;
	// 		parts[i].temp = 1500.0f + 273.15f;
	// 		return 1;
	// 	}
	// }

	return 0;
}

Element_RSTR::~Element_RSTR() {}
