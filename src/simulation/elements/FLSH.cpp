#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_FLSH PT_FLSH 291
Element_FLSH::Element_FLSH()
{
	Identifier = "DEFAULT_PT_FLSH";
	Name = "FLSH";
	Colour = PIXPACK(0xf05b5b);
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
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

	Flammable = 10;
	Explosive = 0;
	Meltable = 0;
	Hardness = 15;

	Weight = 100;

	HeatConduct = 104;
	Description = "Flesh. Requires food, oxygen and water to stay alive, can be cooked.";

	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 200.0f + 273.15f;
	HighTemperatureTransition = PT_FIRE;

	Update = &Element_FLSH::update;
	Graphics = &Element_FLSH::graphics;
}

//#TPT-Directive ElementHeader Element_FLSH static int update(UPDATE_FUNC_ARGS)
int Element_FLSH::update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties
	 * tmp:   Water stored
	 * tmp2:  Food stored
	 * pavg0: Highest temperature
	 * pavg1: Type 0 = inside, 1 = skin, 2 = dead
	 */
	if (parts[i].temp > parts[i].pavg[0])
		parts[i].pavg[0] = parts[i].temp;

	int rx, ry, r, rt;

	for (rx = -1; rx < 2; ++rx)
	for (ry = -1; ry < 2; ++ry)
		if (BOUNDS_CHECK && (rx || ry)) {
			r = pmap[y + ry][x + rx];
			if (!r) continue;
			rt = TYP(r);

			// Alive flesh
			if (parts[i].pavg[1] != 2) {
				// Distribute nutrients
				if (rt == PT_FLSH) {
					int diff = parts[i].tmp - parts[ID(r)].tmp;
					parts[i].tmp -= diff / 2;
					parts[ID(r)].tmp += (diff + 1) / 2;
					diff = parts[i].tmp2 - parts[ID(r)].tmp2;
					parts[i].tmp2 -= diff / 2;
					parts[ID(r)].tmp2 += (diff + 1) / 2;
				}
				// Create skin
			}
		}

	return 0;
}

//#TPT-Directive ElementHeader Element_FLSH static int graphics(GRAPHICS_FUNC_ARGS)
int Element_FLSH::graphics(GRAPHICS_FUNC_ARGS) {
	// TODO move to a create func
	int x = nx, y = ny;
	int gx = x / 50 * 50;
	int gy = y / 50 * 50;
	float theta = x + y;

	int newx = cos(theta) * (x - gx) - sin(theta) * (y - gy) + x;
	int newy = sin(theta) * (x - gx) + cos(theta) * (y - gy) + y;
	x = newx, y = newy;

	int thickness = (x / 4 + y / 4) % 5;
	if ((x + y) % 25 < thickness || abs(x - y) % 25 < thickness) {
		*colr = 255;
		*colg = 255;
		*colb = 255;
	}

	return 0;
}

Element_FLSH::~Element_FLSH() {}
