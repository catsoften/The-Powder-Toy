#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_ANH2 PT_ANH2 304
Element_ANH2::Element_ANH2()
{
	Identifier = "DEFAULT_PT_ANH2";
	Name = "ANH2";
	Colour = PIXPACK(0xbd3335);
	MenuVisible = 1;
	MenuSection = SC_GAS;
	Enabled = 1;

	Advection = 2.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.10f;
	Gravity = 0.00f;
	Diffusion = 3.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 1;

	HeatConduct = 251;
	Description = "Anti-Hydrogen. Annihilates ordinary matter, undergoes fusion at high temperature and pressure.";

	Properties = TYPE_GAS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_ANH2::update;
	// Graphics = &Element_ANH2::graphics;
}

//#TPT-Directive ElementHeader Element_ANH2 static int update(UPDATE_FUNC_ARGS)
int Element_ANH2::update(UPDATE_FUNC_ARGS) {
	int r, rx, ry, rt;
	for (rx = -2; rx < 3; rx++)
	for (ry = -2; ry < 3; ry++)
		if (BOUNDS_CHECK && (rx || ry)) {
			r = pmap[y+ry][x+rx];
			if (!r) continue;
			rt = TYP(r);

			// Stolen from HYGN
			if (sim->pv[y/CELL][x/CELL] > 45.0f) {
				if (parts[ID(r)].temp > 2273.15)
					continue;
			}
			else {
				if (rt == PT_FIRE) {
					if(parts[ID(r)].tmp & 0x02)
						parts[ID(r)].temp = 3473.0f;
					else
						parts[ID(r)].temp = 2473.15f;
					parts[ID(r)].tmp |= 1;
					int ni = sim->create_part(i, x, y,PT_FIRE);
					parts[ni].life = RNG::Ref().between(10, 120);
					parts[i].temp += RNG::Ref().between(0, 99);
					parts[i].tmp |= 1;
					return 1;
				}
				else if ((rt == PT_PLSM && !(parts[ID(r)].tmp & 4)) || (rt == PT_LAVA && parts[ID(r)].ctype != PT_BMTL)) {
					int ni = sim->create_part(i, x, y,PT_FIRE);
					parts[ni].life = RNG::Ref().between(10, 120);
					parts[i].temp += RNG::Ref().between(0, 99);
					parts[i].tmp |= 1;
					return 1;
				}

				// Destroy on contact with ordinary matter
				// TODO dont die on clone or void or pvod or shit
				if (!(sim->elements[rt].Properties & PROP_INDESTRUCTIBLE)) {

				}
			}
		}

	return 0;
}

//#TPT-Directive ElementHeader Element_ANH2 static int graphics(GRAPHICS_FUNC_ARGS)
int Element_ANH2::graphics(GRAPHICS_FUNC_ARGS) {
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_ANH2::~Element_ANH2() {}
