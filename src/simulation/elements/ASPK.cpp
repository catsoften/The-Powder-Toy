#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_ASPK PT_ASPK 283
Element_ASPK::Element_ASPK()
{
	Identifier = "DEFAULT_PT_ASPK";
	Name = "ASPK";
	Colour = PIXPACK(0x45a9c4);
	MenuVisible = 1;
	MenuSection = SC_ELEC;
	Enabled = 1;

	Advection = 0.4f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.2f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 5;
	Hardness = 1;

	Weight = 30;

	HeatConduct = 50;
	Description = "Anti-static powder. Prevents most conductors from conducting spark, blocks lightning. Sticky.";

	Properties = TYPE_PART;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1583.0f;
	HighTemperatureTransition = PT_FIRE;

	Update = &Element_ASPK::update;
	Graphics = &Element_ASPK::graphics;
}

//#TPT-Directive ElementHeader Element_ASPK static int update(UPDATE_FUNC_ARGS)
int Element_ASPK::update(UPDATE_FUNC_ARGS) {
	int rx, ry, r;
	float vx = 0.0f, vy = 0.0f;

	for (rx = -2; rx <= 2; ++rx)
	for (ry = -2; ry <= 2; ++ry)
		if (BOUNDS_CHECK && (rx || ry)) {
			r = pmap[y + ry][x + rx];
			if (!r) r= sim->photons[y + ry][x + rx];
			if (!r) continue;

			if (sim->elements[TYP(r)].Properties & PROP_CONDUCTS) {
				parts[ID(r)].life = 4; // Prevent conduction
				vx += rx;
				vy += ry;
			}
			else if (TYP(r) == PT_ELEC || TYP(r) == PT_APRT || TYP(r) == PT_THDR)
				sim->kill_part(ID(r));
		}

	parts[i].vx += isign(vx);
	parts[i].vy += isign(vy);

	return 0;
}

//#TPT-Directive ElementHeader Element_ASPK static int graphics(GRAPHICS_FUNC_ARGS)
int Element_ASPK::graphics(GRAPHICS_FUNC_ARGS) {
	return 1;
}

Element_ASPK::~Element_ASPK() {}
