#include "simulation/ElementCommon.h"
#include "simulation/Air.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_LEAD() {
	Identifier = "DEFAULT_PT_LEAD";
	Name = "LEAD";
	Colour = PIXPACK(0xD6D6FF);
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
	Enabled = 1;

	Advection = 0.05f;
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
	Description = "Soft metal with a low melting point. Blocks pressure, absorbs energy particles.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC | PROP_DEADLY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 273.15f + 327.0f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS) {
	// Block pressure and air
	sim->air->bmap_blockair[y / CELL][x / CELL] = 1;

	// Absorb energy
	for (int rx = -1; rx <= 1; ++rx)
	for (int ry = -1; ry <= 1; ++ry)
		if (BOUNDS_CHECK) {
			int r = sim->photons[y + ry][x + rx];
			if (!r || TYP(r) == PT_RSPK || TYP(r) == PT_BALI || TYP(r) == PT_JCB1 || TYP(r) == PT_NTRI) continue;

			parts[i].temp += parts[ID(r)].temp * 0.05f;
			sim->kill_part(ID(r));
		}

	return 0;
}
