#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_ERAY PT_ERAY 268
Element_ERAY::Element_ERAY()
{
	Identifier = "DEFAULT_PT_ERAY";
	Name = "ERAY";
	Colour = PIXPACK(0x88d1ba);
	MenuVisible = 1;
	MenuSection = SC_ELEC;
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
	Hardness = 1;

	Weight = 100;

	HeatConduct = 0;
	Description = "Property setter ray. Copies all its properties onto the first particle it hits.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_ERAY::update;
	Graphics = &Element_ERAY::graphics;
}

//#TPT-Directive ElementHeader Element_ERAY static int update(UPDATE_FUNC_ARGS)
int Element_ERAY::update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * - Sparked with PSCN: add property
	 * - Sparked with NSCN: sub property
	 * - Sparked with INWR: ignore properties set to 0
	 * - Otherwise:         Copy all
	 */
	int rx, ry, r;
	for (rx = -1; rx <= 1; ++rx)
	for (ry = -1; ry <= 1; ++ry)
		if (BOUNDS_CHECK && (rx || ry)) {
			r = pmap[y + ry][x + rx];
			if (!r) continue;
			
			if (TYP(r) == PT_SPRK && parts[ID(r)].life == 3) {
				int tx, ty;
				Element_SPDR::intersect_line(sim, x, y, -rx, -ry, tx, ty, 1, PT_NONE);

				int r2 = pmap[ty][tx];
				if (!r2) r2 = sim->photons[ty][tx];
				if (!r2) return 0;

				switch(parts[ID(r)].ctype) {
					case PT_PSCN:
						parts[ID(r2)].temp += parts[i].temp;
						parts[ID(r2)].ctype += parts[i].ctype;
						parts[ID(r2)].life += parts[i].life;
						parts[ID(r2)].tmp += parts[i].tmp;
						parts[ID(r2)].tmp2 += parts[i].tmp2;
						parts[ID(r2)].dcolour += parts[i].dcolour;
						parts[ID(r2)].pavg[0] += parts[i].pavg[0];
						parts[ID(r2)].pavg[1] += parts[i].pavg[1];
						break;
					case PT_NSCN:
						parts[ID(r2)].temp -= parts[i].temp;
						parts[ID(r2)].ctype -= parts[i].ctype;
						parts[ID(r2)].life -= parts[i].life;
						parts[ID(r2)].tmp -= parts[i].tmp;
						parts[ID(r2)].tmp2 -= parts[i].tmp2;
						parts[ID(r2)].dcolour -= parts[i].dcolour;
						parts[ID(r2)].pavg[0] -= parts[i].pavg[0];
						parts[ID(r2)].pavg[1] -= parts[i].pavg[1];
						break;
					case PT_INWR:
						if (parts[i].temp)
							parts[ID(r2)].temp = parts[i].temp;
						if (parts[i].ctype)
							parts[ID(r2)].ctype = parts[i].ctype;
						if (parts[i].life)
							parts[ID(r2)].life = parts[i].life;
						if (parts[i].tmp)
							parts[ID(r2)].tmp = parts[i].tmp;
						if (parts[i].tmp2)
							parts[ID(r2)].tmp2 = parts[i].tmp2;
						if (parts[i].dcolour)	
							parts[ID(r2)].dcolour = parts[i].dcolour;
						if (parts[i].pavg[0])
							parts[ID(r2)].pavg[0] = parts[i].pavg[0];
						if (parts[i].pavg[1])
							parts[ID(r2)].pavg[1] = parts[i].pavg[1];
						break;
					default:
						parts[ID(r2)].temp = parts[i].temp;
						parts[ID(r2)].ctype = parts[i].ctype;
						parts[ID(r2)].life = parts[i].life;
						parts[ID(r2)].tmp = parts[i].tmp;
						parts[ID(r2)].tmp2 = parts[i].tmp2;
						parts[ID(r2)].dcolour = parts[i].dcolour;
						parts[ID(r2)].pavg[0] = parts[i].pavg[0];
						parts[ID(r2)].pavg[1] = parts[i].pavg[1];
						break;
				}
			}
		}

	return 0;
}

//#TPT-Directive ElementHeader Element_ERAY static int graphics(GRAPHICS_FUNC_ARGS)
int Element_ERAY::graphics(GRAPHICS_FUNC_ARGS) {
	return 1;
}

Element_ERAY::~Element_ERAY() {}
