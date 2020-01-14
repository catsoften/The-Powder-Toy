#include "simulation/ElementCommon.h"

#define MAX_L 10000000.0f
#define HIGHEST_EFFECTIVE_RES 10000000.0f

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
	DefaultProperties.pavg[0] = 1.0f;
	DefaultProperties.pavg[1] = MAX_L;

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1273.0f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_INDC::update;
	Graphics = &Element_INDC::graphics;
}

//#TPT-Directive ElementHeader Element_INDC static int update(UPDATE_FUNC_ARGS)
int Element_INDC::update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * pavg0 - inductance
	 * pavg1 - effective resistance
	 * tmp   - 1000 * highest voltage
	 * tmp2  - Is current increasing?
	 */
	if (parts[i].pavg[0] <= 0.001f) // No invalid inductance (too small = division by 0 or overflow)
		parts[i].pavg[0] = 0.001f;

	if (parts[i].pavg[1] < 0.001f) // Avoid effective resistance turning to 0
		parts[i].pavg[1] = 0.001f;
	if (parts[i].pavg[1] > HIGHEST_EFFECTIVE_RES) // Effective resistance can't go to infinity
		parts[i].pavg[1] = HIGHEST_EFFECTIVE_RES; 

	// Currently being charged by a non-voltage source
	int r = sim->photons[y][x];
	float efactor = pow(2.718, 1 / parts[i].pavg[0]);

	// Resisting increasing current
	if (r && TYP(r) == PT_RSPK && parts[ID(r)].tmp != 1) {
		parts[i].pavg[1] /= efactor;

		float current_v = 1000 * parts[ID(r)].pavg[1];
		if (current_v > parts[i].tmp)
			parts[i].tmp = current_v;
		parts[i].tmp2 = 1;
	}
	// Resisting decreasing current
	else if (parts[i].pavg[1] < HIGHEST_EFFECTIVE_RES) {
		if (!r || TYP(r) != PT_RSPK)
			sim->create_part(-3, x, y, PT_RSPK);
		r = sim->photons[y][x];
		parts[ID(r)].tmp = 1;
		parts[ID(r)].life = 2;

		parts[ID(r)].pavg[0] = parts[i].tmp / 1000.0f;
		parts[i].tmp /= efactor;
		parts[i].pavg[1] *= efactor;
		parts[i].tmp2 = 0;
	}
	// // Kill the source, no more net discharge
	else if (parts[i].pavg[1] >= HIGHEST_EFFECTIVE_RES) {
		if (r && TYP(r) == PT_RSPK)
			sim->kill_part(ID(r));
	}

	return 0;
}

//#TPT-Directive ElementHeader Element_INDC static int graphics(GRAPHICS_FUNC_ARGS)
int Element_INDC::graphics(GRAPHICS_FUNC_ARGS) {
	return 1;
}

Element_INDC::~Element_INDC() {}

#undef MAX_L
#undef HIGHEST_EFFECTIVE_RES