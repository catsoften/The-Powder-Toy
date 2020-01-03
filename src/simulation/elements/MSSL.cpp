#include "simulation/ElementCommon.h"

#define DETONATE sim->create_part(-1, x + 1, y + 1, PT_BMTL); \
				 sim->create_part(-1, x + 1, y - 1, PT_BMTL); \
				 sim->create_part(-1, x - 1, y + 1, PT_BMTL); \
				 sim->create_part(-1, x - 1, y - 1, PT_BMTL); \
				 sim->create_part(-1, x, y + 1, PT_BMTL); \
				 sim->create_part(-1, x, y - 1, PT_BMTL); \
				 sim->create_part(-1, x - 1, y, PT_BMTL); \
				 sim->create_part(-1, x + 1, y, PT_BMTL);

//#TPT-Directive ElementClass Element_MSSL PT_MSSL 237
Element_MSSL::Element_MSSL()
{
	Identifier = "DEFAULT_PT_MSSL";
	Name = "MSSL";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 0;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.00f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.5f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 5;

	Weight = 100;

	HeatConduct = 35;
	Description = "Guided missile. Hidden element.";

	DefaultProperties.life = 10;
	Properties = TYPE_PART | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 2887.15f;
	HighTemperatureTransition = PT_BMTL;

	Update = &Element_MSSL::update;
	Graphics = &Element_MSSL::graphics;
}

//#TPT-Directive ElementHeader Element_MSSL static int update(UPDATE_FUNC_ARGS)
int Element_MSSL::update(UPDATE_FUNC_ARGS) {
	if (parts[i].life > 0) { // Not primed yet
		parts[i].life--;
		return 0;
	}

	// Align self towards target
	float angle = atan2(parts[i].pavg[1] - y, parts[i].pavg[0] - x);
	parts[i].vx += 2.0f * cos(angle);
	parts[i].vy += 2.0f * sin(angle);

	int rx, ry, rt, r;
	int tx = parts[i].x + 0.5f + parts[i].vx,
		ty = parts[i].y + 0.5f + parts[i].vy;

	// Create exhaust
	int px = parts[i].x + 0.5f - parts[i].vx,
		py = parts[i].y + 0.5f - parts[i].vy;
	sim->create_part(-1, px, py, PT_SMKE);

	// Should it explode now?
	int dis = (parts[i].pavg[0] - x) * (parts[i].pavg[0] - x) + (parts[i].pavg[1] - y) * (parts[i].pavg[1] - y);
	if (dis <= 4) {
		sim->part_change_type(i, x, y, PT_BOMB);
		DETONATE
		return 0;
	}

	// If target location is a liquid or GAS swap places
	r = pmap[ty][tx];
	rt = TYP(r);
	if (r && (sim->elements[rt].Properties & TYPE_LIQUID || sim->elements[rt].Properties & TYPE_GAS)) {
		parts[i].x = parts[ID(r)].x;
		parts[i].y = parts[ID(r)].y;
		parts[ID(r)].x = x;
		parts[ID(r)].y = y;
		pmap[y][x] = r;
		pmap[ty][tx] = PMAP(i, parts[i].type);
		return 0;
	}

	// Check for nearby solids and powders
	for (rx = -1; rx <= 1; ++rx)
		for (ry = -1; ry <= 1; ++ry)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = pmap[y + ry][x + rx];
				if (!r) continue;
				rt = TYP(r);
				
				// Explode on contact
				if (rt != PT_MSSL && (sim->elements[rt].Properties & TYPE_SOLID || sim->elements[rt].Properties & TYPE_PART)) {
					sim->part_change_type(i, x, y, PT_BOMB);
					DETONATE
					return 0;
				}
			}

	return 0;
}

//#TPT-Directive ElementHeader Element_MSSL static int graphics(GRAPHICS_FUNC_ARGS)
int Element_MSSL::graphics(GRAPHICS_FUNC_ARGS) {
	return 1;
}

Element_MSSL::~Element_MSSL() {}
