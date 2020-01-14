#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_PAPR PT_PAPR 271
Element_PAPR::Element_PAPR()
{
	Identifier = "DEFAULT_PT_PAPR";
	Name = "PAPR";
	Colour = PIXPACK(0xFAFAFA);
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
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 50;
	Explosive = 0;
	Meltable = 0;
	Hardness = 50;

	Weight = 100;

	HeatConduct = 164;
	Description = "Paper. Can be stained, dissolves in water, burns quickly.";

	Properties = TYPE_SOLID | PROP_NEUTPENETRATE | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 232.8f + 273.15f; // 451 F
	HighTemperatureTransition = PT_FIRE;

	Update = &Element_PAPR::update;
	Graphics = &Element_PAPR::graphics;
}

//#TPT-Directive ElementHeader Element_PAPR static int update(UPDATE_FUNC_ARGS)
int Element_PAPR::update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * - life:    how much water it has
	 * - dcolour: current stained color
	 */

	// Chance to dissolve if too wet
	if (parts[i].life > 30000 && RNG::Ref().chance(1, 500)) {
		sim->kill_part(i);
		return 1;
	}

	int rx, ry, r, rt;
	int ri = PIXR(parts[i].dcolour), gi = PIXG(parts[i].dcolour), bi = PIXB(parts[i].dcolour);

	for (rx = -1; rx <= 1; ++rx)
	for (ry = -1; ry <= 1; ++ry)
		if (BOUNDS_CHECK && (rx || ry)) {
			r = pmap[y + ry][x + rx];
			if (!r) continue;
			rt = TYP(r);

			// Diffuse color to neighbours
			if (rt == PT_PAPR && parts[i].dcolour) {
				int ro = PIXR(parts[ID(r)].dcolour);
				int go = PIXG(parts[ID(r)].dcolour);
				int bo = PIXB(parts[ID(r)].dcolour);
				int nc = 0xFF000000 + 0xFF00 * (ro + ri) / 2 + 0xFF * (go + gi) / 2 + (bo + bi) / 2;

				parts[i].dcolour = nc;
				parts[ID(r)].dcolour = nc;
			}

			// Stain self
			if (sim->elements[rt].Properties & TYPE_LIQUID || sim->elements[rt].Properties & TYPE_PART) {
				int color = sim->elements[rt].Colour;
				int ro = PIXR(color), go = PIXG(color), bo = PIXB(color);

				// Either self is not stained, or staining with same color
				if (parts[i].dcolour == 0 || (ro == ri && go == gi && bo == bi)) {
					
				}
			}

			// Get "wetter" with liquid
			if (sim->elements[rt].Properties & TYPE_LIQUID || rt == PT_WTRV) {
				parts[i].life += 500;
				sim->kill_part(ID(r));
				continue;
			}

			// Diffuse wetness
			if (rt == PT_PAPR && parts[ID(r)].life < parts[i].life) {
				int lose = std::min(parts[i].life, 1000);
				parts[i].life -= lose;
				parts[ID(r)].life += lose;
			}
		}

	return 0;
}

//#TPT-Directive ElementHeader Element_PAPR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_PAPR::graphics(GRAPHICS_FUNC_ARGS) {
	int darker = cpart->life / 200;
	if (darker > 50)
		darker = 50;
	*colr -= darker;
	*colg -= darker;
	*colb -= darker;

	return 0;
}

Element_PAPR::~Element_PAPR() {}
