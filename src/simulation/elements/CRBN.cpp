#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_CRBN PT_CRBN 215
Element_CRBN::Element_CRBN()
{
	Identifier = "DEFAULT_PT_CRBN";
	Name = "CRBN";
	Colour = PIXPACK(0x444444);
	MenuVisible = 1;
	MenuSection = SC_POWDERS;
	Enabled = 1;

	Advection = 0.4f;
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
	PhotonReflectWavelengths = 0x00000000;

	Weight = 90;

	HeatConduct = 10;
	Description = "Carbon dust. Turns into RDMD under heat and pressure, superconducts when cold.";

	Properties = TYPE_PART | PROP_CONDUCTS | PROP_NEUTABSORB | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 3600.0f + 273.15f;
	HighTemperatureTransition = PT_CO2;

	Update = &Element_CRBN::update;
	Graphics = &Element_CRBN::graphics;
	Create = &Element_CLST::create;
}

//#TPT-Directive ElementHeader Element_CRBN static int update(UPDATE_FUNC_ARGS)
int Element_CRBN::update(UPDATE_FUNC_ARGS)
{
	// Stay still if tmp2
	if (parts[i].tmp2)
		parts[i].vx = parts[i].vy = 0;

	// Solidify into realistic diamond
	if (parts[i].temp > 273.15f + 2500.0f && sim->pv[y/CELL][x/CELL] > 100.0f && RNG::Ref().chance(1, 20)) {
		sim->part_change_type(i, parts[i].x, parts[i].y, PT_RDND);
		Element_RDND::create(sim, i, x, y, PT_RDND, -1);
		return 0;
	}

	int rx, ry, r, rt;
	bool seen_h2 = false, seen_fire = false, seen_sprk = false;
	int crbn_count = 0;

	for (rx = -1; rx < 2; rx++)
		for (ry = -1; ry < 2; ry++)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = pmap[y + ry][x + rx];
				if (!r) continue;
				rt = TYP(r);

				if (rt == PT_FIRE || rt == PT_PLSM)
					seen_fire = true;
				else if (rt == PT_H2)
					seen_h2 = true;
				else if (rt == PT_SPRK)
					seen_sprk = true;
				else if (rt == PT_CRBN)
					++crbn_count;

				// Solidify
				else if (rt == PT_PSTE) {
					parts[i].tmp2 = 1;
					sim->kill_part(ID(r));
				}

				// Purify water
				else if (rt == PT_WATR)
					sim->part_change_type(ID(r), parts[ID(r)].x, parts[ID(r)].y, PT_DSTW);
				else if (rt == PT_SLTW)
					sim->part_change_type(ID(r), parts[ID(r)].x, parts[ID(r)].y,
						RNG::Ref().chance(1, 2) ? PT_SALT : PT_DSTW);

				// Convert molten IRON into METL (steel)
				else if (rt == PT_LAVA && parts[ID(r)].ctype == PT_IRON) {
					parts[ID(r)].ctype = PT_METL;
					if (RNG::Ref().chance(1, 5)) {
						sim->kill_part(i);
						return 0;
					}
				}
			}

	// H2 + FIRE + CRBN = GAS
	if (seen_h2 && seen_fire)
		sim->part_change_type(i, parts[i].x, parts[i].y, PT_GAS);
	// FIRE + CRBN = CO2
	else if (seen_fire && RNG::Ref().chance(1, 20))
		sim->part_change_type(i, parts[i].x, parts[i].y, PT_CO2);
	// Thin wires superconduct
	else if (parts[i].temp < 100.0f && seen_sprk && crbn_count <= 2) {
		sim->FloodINST(x, y, PT_SPRK, PT_CRBN);
	}
	return 0;
}

//#TPT-Directive ElementHeader Element_CRBN static int graphics(GRAPHICS_FUNC_ARGS)
int Element_CRBN::graphics(GRAPHICS_FUNC_ARGS) {
	int z = (cpart->tmp - 5) * 5; // Speckles!
	*colr += z;
	*colg += z;
	*colb += z;

	return 0;
}

Element_CRBN::~Element_CRBN() {}
