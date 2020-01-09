#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_COPR PT_COPR 251
Element_COPR::Element_COPR() {
	Identifier = "DEFAULT_PT_COPR";
	Name = "COPR";
	Colour = PIXPACK(0xE39E59);
	MenuVisible = 1;
	MenuSection = SC_ELECTROMAG;
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
	Meltable = 1;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "Copper. Great conductor, can be used as a cathode in a battery.";
	PhotonReflectWavelengths = 0xFFA0000;

	Properties = TYPE_SOLID|PROP_CONDUCTS|PROP_LIFE_DEC|PROP_HOT_GLOW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 1.0f;
	HighPressureTransition = ST;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1085.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_COPR::update;
	Graphics = &Element_COPR::graphics;
}

//#TPT-Directive ElementHeader Element_COPR static int update(UPDATE_FUNC_ARGS)
int Element_COPR::update(UPDATE_FUNC_ARGS) {
	int rx, ry, r;
	static int checkCoordsX[] = { -4, 4, 0, 0 };
	static int checkCoordsY[] = { 0, 0, -4, 4 };

	// Fast SPRK code, stolen from GOLD
	if (!parts[i].life) {
		for(int j = 0; j < 4; j++){
			rx = checkCoordsX[j];
			ry = checkCoordsY[j];
			if (BOUNDS_CHECK) {
				r = pmap[y+ry][x+rx];
				if (!r) continue;
				if (TYP(r) == PT_SPRK && parts[ID(r)].life && parts[ID(r)].life < 4) {
					sim->part_change_type(i, x, y, PT_SPRK);
					parts[i].life = 4;
					parts[i].ctype = PT_COPR;
				}
			}
		}
	}

	return 0;
}

//#TPT-Directive ElementHeader Element_COPR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_COPR::graphics(GRAPHICS_FUNC_ARGS) {
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_COPR::~Element_COPR() {}
