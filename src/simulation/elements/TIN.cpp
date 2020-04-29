#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_TIN() {
	Identifier = "DEFAULT_PT_TIN";
	Name = "TIN";
	Colour = PIXPACK(0xFFFAEB);
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
	Hardness = 0;

	Weight = 100;

	HeatConduct = 180;
	Description = "Tin. Poorer heat conductor than most metals, has a low melting point.";
	PhotonReflectWavelengths = 0xFFFFFFF;

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 1.0f;
	HighPressureTransition = ST;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 232.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * - tmp: is it an oxide layer
	 */
	
	// Break (> 5 pressure)
	if (sim->pv[y / CELL][x / CELL] > 5.0f) {
		parts[i].ctype = PT_TIN;
		sim->part_change_type(i, x, y, PT_BRKN);
		return 1;
	}

	for (int rx = -1; rx <= 1; rx++)
	for (int ry = -1; ry <= 1; ry++)
		if (BOUNDS_CHECK && (rx || ry)) {
			int r = pmap[y + ry][x + rx];
			if (!r) {
				if (RNG::Ref().chance(1, 6))
					parts[i].tmp = 1;
				continue;
			}

			// Undo corrosion of IRON (BMTL w/ tmp + 1)
			if (TYP(r) == PT_BMTL && parts[ID(r)].tmp) {
				parts[ID(r)].tmp = 0;
				sim->part_change_type(ID(r), x + rx, y + ry, PT_IRON);
			}

			// Only be dissolved if not an oxide layer
			else if (parts[i].tmp != 1 && (TYP(r) == PT_ACID || TYP(r) == PT_CAUS) && RNG::Ref().chance(1, 100)) {
				sim->kill_part(i);
				sim->kill_part(ID(r));
				continue;
			}

			// Superconduction
			else if (parts[i].temp < 3.72f && TYP(r) == PT_SPRK)
				sim->FloodINST(x, y, PT_TIN);
		}

	return 0;
}
