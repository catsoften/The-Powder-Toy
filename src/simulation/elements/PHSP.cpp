#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_PHSP PT_PHSP 281
Element_PHSP::Element_PHSP()
{
	Identifier = "DEFAULT_PT_PHSP";
	Name = "PHSP";
	Colour = PIXPACK(0xedd5d3);
	MenuVisible = 1;
	MenuSection = SC_EXPLOSIVE;
	Enabled = 1;

	Advection = 0.3f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.3f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 2;
	PhotonReflectWavelengths = 0xFFFFFFFF;

	Weight = 90;

	HeatConduct = 150;
	Description = "Phosphorus. Highly flammable powder.";

	Properties = TYPE_PART | PROP_SPARKSETTLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 280.5f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_PHSP::update;
	Graphics = &Element_PHSP::graphics;
}

//#TPT-Directive ElementHeader Element_PHSP static int update(UPDATE_FUNC_ARGS)
int Element_PHSP::update(UPDATE_FUNC_ARGS) {
	/**
	 * life: Fire burn
	 * tmp:  Phot absorb timer
	 */
	if (parts[i].life == 1) {
		sim->kill_part(i);
		return 1;
	}
	if (parts[i].life > 1)
		parts[i].life--;
	if (parts[i].tmp > 0)
		parts[i].tmp--;

	int rx, ry, r;
	for (rx = -1; rx <= 1; ++rx)
	for (ry = -1; ry <= 1; ++ry)
		if (BOUNDS_CHECK && (rx || ry)) {
			r = pmap[y + ry][x + rx];

			// Ignite
			if (TYP(r) == PT_FIRE || TYP(r) == PT_PLSM || TYP(r) == PT_LAVA) {
				parts[i].life += RNG::Ref().between(0, 5);
			}
			// Burn
			else if (parts[i].life > 0 && !r) {
				int ni = sim->create_part(-1, x + rx, y + ry, RNG::Ref().chance(1, 3) ? PT_EMBR : PT_FIRE);
				if (ni > -1) {
					parts[ni].life = RNG::Ref().between(50, 200);
					parts[ni].vx = rx;
					parts[ni].vy = ry;
					parts[ni].temp = parts[i].temp + 300.0f;
					parts[ni].dcolour = 0xffffb37d;
				}
			}
			// Fertilize plnt
			else if (TYP(r) == PT_PLNT) {
				sim->part_change_type(ID(r), x + rx, y + ry, PT_VINE);
				sim->kill_part(i);
				return 1;
			}

			// Absorb PHOT
			r = sim->photons[y + ry][x + rx];
			if (TYP(r) == PT_PHOT) {
				sim->kill_part(ID(r));
				parts[i].tmp += 30;
			}
		}

	return 0;
}

//#TPT-Directive ElementHeader Element_PHSP static int graphics(GRAPHICS_FUNC_ARGS)
int Element_PHSP::graphics(GRAPHICS_FUNC_ARGS) {
	if (cpart->tmp > 0) {
		*firea = 150;
		*firer = *fireg = *fireb = 255;
		*pixel_mode |= FIRE_ADD;
	}

	return 0;
}

Element_PHSP::~Element_PHSP() {}
