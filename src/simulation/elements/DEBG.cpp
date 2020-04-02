#include "simulation/ElementCommon.h"

#include "simulation/circuits/circuits.h"
#include "simulation/circuits/framework.h"
#include "simulation/circuits/resistance.h"
#include "simulation/magnetics/magnetics.h"

//#TPT-Directive ElementClass Element_DEBG PT_DEBG 272
Element_DEBG::Element_DEBG()
{
	Identifier = "DEFAULT_PT_DEBG";
	Name = "DEBG";
	Colour = PIXPACK(0xFF0000);
	MenuVisible = 1;
	MenuSection = SC_CRACKER2;
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
	Hardness = 0;

	Weight = 100;

	HeatConduct = 0;
	Description = "Debugger (for mod testing lol). Set tmp to change debugging mode.";

	Properties = TYPE_SOLID | PROP_INDESTRUCTIBLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_DEBG::update;
	Graphics = &Element_DEBG::graphics;
}

//#TPT-Directive ElementHeader Element_DEBG static int update(UPDATE_FUNC_ARGS)
int Element_DEBG::update(UPDATE_FUNC_ARGS) {
	/**
	 * Debug types:
	 * 1 - Get faraday wall ID
	 * 2 - List circuits
	 */

	return 0;
}

//#TPT-Directive ElementHeader Element_DEBG static int graphics(GRAPHICS_FUNC_ARGS)
int Element_DEBG::graphics(GRAPHICS_FUNC_ARGS) {
	if (cpart->tmp == 1)
		ren->drawtext(nx, ny - 5, String::Build("ID: ", ren->sim->faraday_map[ny / CELL][nx / CELL]), 255, 255, 255, 255);
	else if (cpart->tmp == 2) {
		int tx = ren->mousePos.X,
			ty = ren->mousePos.Y;
		ren->drawtext(nx, ny, String::Build(all_circuits.size(), circuit_map[ID(ren->sim->photons[ty][tx])] != nullptr ? 1 : 0), 255, 255, 255, 255);


		int y = ny + 12;
		for (auto c : all_circuits) {
			ren->drawtext(nx, y, String::Build("Branches: ", c->branch_cache_size()), 255, 255, 255, 255);
			y += 12;
		}
	}

	return 0;
}

Element_DEBG::~Element_DEBG() {}
