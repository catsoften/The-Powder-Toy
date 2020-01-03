#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_BOWR PT_BOWR 200
Element_BOWR::Element_BOWR()
{
	Identifier = "DEFAULT_PT_BOWR";
	Name = "BOWR";
	Colour = PIXPACK(0xF5B400);
	MenuVisible = 1;
	MenuSection = SC_CRACKER2;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.1f;
	Gravity = 0.0f;
	Diffusion = 0.75f;
	HotAir = 0.001f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 500;

	DefaultProperties.temp = 9999.0f;
	HeatConduct = 100;
	Description = "Bowserinator, highly destructive singularity";

	Properties = TYPE_GAS | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPL;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITL;
	HighTemperatureTransition = NT;

	Update = &Element_BOWR::update;
	Graphics = &Element_BOWR::graphics;
}

//#TPT-Directive ElementHeader Element_BOWR static int update(UPDATE_FUNC_ARGS)
int Element_BOWR::update(UPDATE_FUNC_ARGS)
{
	sim->gravmap[(y / CELL)*(XRES / CELL) + (x / CELL)] = 0.2f*(500);
	sim->create_part(-1, parts[i].x - 1, parts[i].y - 1, PT_THDR);
	sim->create_part(-1, parts[i].x - 1, parts[i].y + 1, PT_PLSM);
	sim->create_part(-1, parts[i].x + 1, parts[i].y + 1, PT_PLSM);
	sim->create_part(-1, parts[i].x + 1, parts[i].y - 1, PT_CO2);
	if (rand() % 200 == 0){
		sim->create_part(-1, parts[i].x - 1, parts[i].y - 1, PT_LIGH);
	}
	parts[i].temp = 999999.0f;

	int r, rx, ry;
	for (rx = -1; rx < 2; rx++)
		for (ry = -1; ry < 2; ry++)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = pmap[y + ry][x + rx];
				if (!r || TYP(r) == PT_BOWR) continue;
				if (rand() % 20 == 0)
					sim->kill_part(ID(r));
			}
	return 0;
}

//#TPT-Directive ElementHeader Element_BOWR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_BOWR::graphics(GRAPHICS_FUNC_ARGS)
{
	return 1;
}

Element_BOWR::~Element_BOWR() {}
