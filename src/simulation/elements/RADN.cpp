#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static int frozen_graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_RADN() {
	Identifier = "DEFAULT_PT_RADN";
	Name = "RADN";
	Colour = PIXPACK(0xFEFE00);
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 1.0f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.1f;
	Gravity = 0.3f;
	Diffusion = 0.18f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 5;

	DefaultProperties.temp = R_TEMP + 2.0f + 273.15f;
	HeatConduct = 42;
	Description = "Radon, unstable gas. Decays with slow, hot neutrons.";
	Properties = TYPE_GAS | PROP_RADIOACTIVE | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = 273.15f - 71.0f; // Technically there's a liquid state but fuck that
	LowTemperatureTransition = PT_ICEI;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	DefaultProperties.tmp = 135;

	Update = &update;
	Graphics = &graphics;
	FrozenGraphics = &frozen_graphics;
	Create = &create;
}

static int update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * tmp  - neutrons left
	 * tmp2 - Glow?
	 */
	bool decay_this_frame = RNG::Ref().chance(1, 30000);

	for (int rx = -1; rx <= 1; rx++)
	for (int ry = -1; ry <= 1; ry++)
		if (BOUNDS_CHECK && (rx || ry)) {
			int r = sim->photons[y + ry][x + rx];
			if (!r) continue;
			int rt = TYP(r);

			if ((rt == PT_NEUT || rt == PT_PROT) &&
					(parts[ID(r)].vx * parts[ID(r)].vx + parts[ID(r)].vy * parts[ID(r)].vy) < 1.7f * 1.7f &&
					 parts[ID(r)].temp > 1000.0f) {
				decay_this_frame = true;
				goto end;
			}
		}
	end:;

	if (decay_this_frame) {
		parts[i].tmp -= 3;
		parts[i].temp += 2.0f;
		for (int i = 0; i < 3; i++)
			sim->create_part(-3, x, y, PT_NEUT);
	}

	if (parts[i].tmp <= 0) {
		sim->part_change_type(i, x, y, PT_POLO);
		return 1;
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS) {
	*pixel_mode &= ~PMODE;
	*firer = *colr / 2;
	*fireg = *colg / 2;
	*fireb = *colb / 2;
	*firea = 80;
	*pixel_mode |= cpart->tmp2 ? FIRE_ADD : FIRE_BLEND;

	return 0;
}

static int frozen_graphics(GRAPHICS_FUNC_ARGS) {
	// Orange-red
	*colr = 0xFF;
	*colg = 0x55;
	*colb = 0x0;

	if (cpart->type == PT_ICEI) {
		*pixel_mode &= ~PMODE;
		*firer = *colr / 1.5f;
		*fireg = *colg / 1.5f;
		*fireb = *colb / 1.5f;
		*firea = 20;
		*pixel_mode |= FIRE_BLEND;
	}
	return 1;
}

static void create(ELEMENT_CREATE_FUNC_ARGS) {
	sim->parts[i].tmp2 = RNG::Ref().chance(1, 2);
}
