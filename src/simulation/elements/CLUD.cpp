#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_CLUD PT_CLUD 236
Element_CLUD::Element_CLUD()
{
	Identifier = "DEFAULT_PT_CLUD";
	Name = "CLUD";
	Colour = PIXPACK(0xCCCCCC);
	MenuVisible = 1;
	MenuSection = SC_GAS;
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
	Meltable = 0;
	Hardness = 0;

	Weight = 10;

	HeatConduct = 0;
	Description = "Cloud. Weather depends on ctype. (SNOW / WATR / LIGH / LAVA)";

	Properties = TYPE_GAS | PROP_NOCTYPEDRAW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_CLUD::update;
	Graphics = &Element_CLUD::graphics;
}

//#TPT-Directive ElementHeader Element_CLUD static int update(UPDATE_FUNC_ARGS)
int Element_CLUD::update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * - ctype
	 * 	- WATR - Rain, gray cloud
	 *  - LIGH - Thunderstorm, LIGH randomly. Lights up randomly
	 * 			 may make BALI
	 *  - SNOW - Snow storm
	 *  - GLAS - GLAS (Molten if cloud is hot enough)
	 *  - LAVA - Molten LAVA
	 */

	// Rain
	if (RNG::Ref().chance(1, 1500)) {
		int ni = -1;
		if (parts[i].ctype == PT_LIGH || parts[i].ctype == PT_THDR || parts[i].ctype == PT_WATR || parts[i].ctype == PT_DSTW)
			ni = sim->create_part(-3, x, y, PT_WATR);
		else if (parts[i].ctype == PT_GLAS)
			ni = sim->create_part(-3, x, y, PT_BGLA);
		else if (parts[i].ctype == PT_LAVA)
			ni = sim->create_part(-3, x, y, PT_LAVA);
		else if (parts[i].ctype == PT_SNOW || parts[i].ctype == PT_ICEI)
			ni = sim->create_part(-3, x, y, PT_SNOW);

		if (ni > 0) // Rain = cloud temp
			parts[ni].temp = parts[i].temp;
	}

	// Lightning
	if (RNG::Ref().chance(1, 500000) && (parts[i].ctype == PT_LIGH || parts[i].ctype == PT_THDR)) {
		sim->create_part(-3, x, y, PT_LIGH);
	}

	// BALI (super rare!)
	if (RNG::Ref().chance(1, 500000000) && (parts[i].ctype == PT_LIGH || parts[i].ctype == PT_THDR)) {
		sim->create_part(-3, x, y, PT_BALI);
	}

	return 0;
}

//#TPT-Directive ElementHeader Element_CLUD static int graphics(GRAPHICS_FUNC_ARGS)
int Element_CLUD::graphics(GRAPHICS_FUNC_ARGS) {
	// Vary color depending on type of cloud
	float m = 1.0f;

	// Dark storm cloud
	if (cpart->ctype == PT_LIGH || cpart->ctype == PT_THDR) {
		m = 0.2f;
	}
	// Rain
	else if (cpart->ctype == PT_WATR || cpart->ctype == PT_DSTW) {
		m = 0.5f;
	}
	// Snow
	else if (cpart->ctype == PT_ICEI || cpart->ctype == PT_SNOW) {
		m = 0.8f;
	}
	// Molten
	else if (cpart->ctype == PT_GLAS || cpart->ctype == PT_LAVA) {
		*colr *= 0.5f;
		*colg *= 0.2f;
		*colb *= 0.1f;
	}

	if (m != 1.0f) {
		*colr *= m;
		*colg *= m;
		*colb *= m;
	}

	// Gas effect
	*pixel_mode &= ~PMODE;
	*pixel_mode |= FIRE_BLEND;
	*firer = *colr * 0.7f;
	*fireg = *colg * 0.7f;
	*fireb = *colb * 0.7f;
	*firea = 125;
	*pixel_mode |= DECO_FIRE;

	return 0;
}

Element_CLUD::~Element_CLUD() {}
