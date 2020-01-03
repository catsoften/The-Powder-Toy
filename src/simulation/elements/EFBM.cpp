#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_EFBM PT_EFBM 190
Element_EFBM::Element_EFBM()
{
	Identifier = "DEFAULT_PT_EFBM";
	Name = "EFBM";
	Colour = PIXPACK(0x000077);
	MenuVisible = 0;
	MenuSection = SC_ELEC;
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

	HeatConduct = 1;
	Description = "Excursion Funnel Beam.";

	Properties = TYPE_ENERGY | PROP_LIFE_DEC | PROP_LIFE_KILL;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	DefaultProperties.life = 1;

	Update = &Element_EFBM::update;
	Graphics = &Element_EFBM::graphics;
}

//#TPT-Directive ElementHeader Element_EFBM static int update(UPDATE_FUNC_ARGS)
int Element_EFBM::update(UPDATE_FUNC_ARGS) {
	/* Properties:
	 * tmp: direction, see EXFN for values
	 * 		0 = left, 1 = up, 2 = right, 3 = down
	 * tmp2: pull direction, 0 = in direction of tmp, 1 = opposite
	 * ctype: graphics
	 */
	int r, rx, ry, id;
	int dx, dy;
	if (parts[i].tmp == 0 || parts[i].tmp == 2) {
		dx = parts[i].tmp == 0 ? -1 : 1;
		dy = 0;
	} else {
		dy = parts[i].tmp == 1 ? -1 : 1;
		dx = 0;
	}

	for (rx = -1; rx <= 1; rx++)
		for (ry = -1; ry <= 1; ry++)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = sim->photons[y + ry][x + rx];
				if (!r) continue;
				
				// "Flow" the ctype animation in the direction of tmp2
				if (rx == -dx && ry == -dy && TYP(r) == PT_EFBM) {
					if (parts[ID(r)].ctype >= 1) {
						parts[ID(r)].ctype = 0;
						parts[i].ctype = 1;
					}
				}
			}

	return 0;
}

//#TPT-Directive ElementHeader Element_EFBM static int graphics(GRAPHICS_FUNC_ARGS)
int Element_EFBM::graphics(GRAPHICS_FUNC_ARGS) {
	*pixel_mode = PMODE_BLEND;

	// Color depends on direction
	if (cpart->tmp2 == 0) {
		*colr = 84; *colg = 127; *colb = 255;
	} else {
		*colr = 255; *colg = 152; *colb = 84;
	}

	// Alpha depends on ctype
	// 0 = almost empty
	// 1 = normal wave
	// 2 = offset wave (lighter)
	// 3 = scan beam
	if (cpart->ctype == 0)
		*cola = 0;
	else if (cpart->ctype == 1)
		*cola = 255;
	else if (cpart->ctype == 2)
		*cola = 80;
	else
		*cola = 80;

	// Glow if not main beam filler
	if (cpart->ctype != 0)
		*pixel_mode = PMODE_BLEND | PMODE_GLOW;
	return 0;
}

Element_EFBM::~Element_EFBM() {}
