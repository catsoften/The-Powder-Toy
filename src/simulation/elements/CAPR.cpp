#include "simulation/ElementCommon.h"

#define MAX_C 10000000.0f // Also check in GameView.cpp
#define LOWEST_EFFECTIVE_RES 0.001f // Also check in GameView.cpp

//#TPT-Directive ElementClass Element_CAPR PT_CAPR 263
Element_CAPR::Element_CAPR() {
	Identifier = "DEFAULT_PT_CAPR";
	Name = "CPTR";
	Colour = PIXPACK(0x82a5cf);
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
	Description = "Capacitor. Stores charge from current and releases it when not charging. pavg0 = capacitance. Use 1px.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;
	DefaultProperties.pavg[0] = 10.0f;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1273.0f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_CAPR::update;
	Graphics = &Element_CAPR::graphics;
}

//#TPT-Directive ElementHeader Element_CAPR static int update(UPDATE_FUNC_ARGS)
int Element_CAPR::update(UPDATE_FUNC_ARGS) {
	/**
	 * pavg0 = capacitance
	 * pavg1 = effective resistance
	 * tmp   = 1000 * voltage while charging
	 * tmp2  = is it charging right now
	 */

	if (parts[i].pavg[0] <= 0.001f) // No invalid capacitance (too small = division by 0 or overflow)
		parts[i].pavg[0] = 0.001f; 

	if (parts[i].pavg[1] <= LOWEST_EFFECTIVE_RES) // Effective resistance can't be "0"
		parts[i].pavg[1] = LOWEST_EFFECTIVE_RES; 

	// Currently being charged by a non-voltage source
	int r = sim->photons[y][x];
	float efactor = pow(2.718, 1 / parts[i].pavg[0]);

	if (r && TYP(r) == PT_RSPK) {
		float current = sim->parts[ID(r)].pavg[1];
		parts[i].pavg[0] -= current * 0.05f;
	}

	// if (r && TYP(r) == PT_RSPK && parts[ID(r)].tmp != 1) {
	// 	parts[i].pavg[1] *= efactor;
	// 	if (parts[i].pavg[1] > MAX_C)
	// 		parts[i].pavg[1] = MAX_C;
	// 	parts[i].tmp = 1000 * parts[ID(r)].pavg[0];
	// 	parts[i].tmp2 = 1;
	// }
	// // Discharging
	// else if (parts[i].pavg[1] > LOWEST_EFFECTIVE_RES) {
	// 	if (!r || TYP(r) != PT_RSPK)
	// 		sim->create_part(-3, x, y, PT_RSPK);
	// 	r = sim->photons[y][x];
	// 	parts[ID(r)].tmp = 1;
	// 	parts[ID(r)].life = 2;

	// 	parts[ID(r)].pavg[0] = parts[i].tmp / 1000.0f; // Set voltage based on charge
	// 	parts[i].pavg[1] /= efactor;
	// 	parts[i].tmp /= efactor;
	// 	parts[i].tmp2 = 0;
	// }
	// // Kill the source, discharge over
	// else if (parts[i].pavg[1] <= LOWEST_EFFECTIVE_RES) {
	// 	if (r && TYP(r) == PT_RSPK)
	// 		sim->kill_part(ID(r));
	// }

	return 0;
}

//#TPT-Directive ElementHeader Element_CAPR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_CAPR::graphics(GRAPHICS_FUNC_ARGS) {
	return 1;
}

Element_CAPR::~Element_CAPR() {}

#undef MAX_C
#undef LOWEST_EFFECTIVE_RES