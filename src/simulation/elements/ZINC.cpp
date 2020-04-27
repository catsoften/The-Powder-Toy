#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_ZINC() {
	Identifier = "DEFAULT_PT_ZINC";
	Name = "ZINC";
	Colour = PIXPACK(0xBADEE3);
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
	Description = "Zinc. Slightly brittle metal, has a low melting point. Can be used as an anode in a battery.";

	Properties = TYPE_SOLID|PROP_CONDUCTS|PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 1.0f;
	HighPressureTransition = ST;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 419.5f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS) {
	// 5 pressure, + 1 for every 20 degrees above 0 C
	if (sim->pv[y / CELL][x / CELL] > 5.0f + (parts[i].temp - 273.15f) / 20.0f && RNG::Ref().chance(1, 10)) {
		sim->part_change_type(i, x, y, PT_BRMT);
		return 1;
	}
	return 0;
}
