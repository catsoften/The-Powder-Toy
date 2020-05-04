#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_AQRG() {
	Identifier = "DEFAULT_PT_AQRG";
	Name = "AQRG";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_LIQUID;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;
	PhotonReflectWavelengths = 0x1FE001FE; // TODO CHANGE

	Weight = 10;
	HeatConduct = 34;
	Description = "Aqua regia. Can dissolve metals.";

	Properties = TYPE_LIQUID | PROP_DEADLY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	DefaultProperties.life = 75;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS) {
	// update code here
	/*
	BKSD - neturalize acids like AQUA REGIA
- dissolve coper, bRNZ, etc... too
- acid, dissolves most normal stuff
- slowly breaks down into CAUS
*/

	for (int rx = -1; rx <= 1; rx++)
	for (int ry = -1; ry <= 1; ry++)
		if (BOUNDS_CHECK && (rx || ry)) {
			int r = pmap[y + ry][x + rx];
			if (!r) continue;
			int rt = TYP(r);

			// Base neutralizes
			if (rt == PT_BAKS || rt == PT_SOAP) {
				parts[i].life--;
				if (RNG::Ref().chance(1, 75))
					sim->kill_part(ID(r));
			}
		}

	if (parts[i].life < 0)
		parts[i].life = 0;

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS) {
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}
