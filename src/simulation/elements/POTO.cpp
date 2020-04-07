#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_POTO PT_POTO 299
Element_POTO::Element_POTO()
{
	Identifier = "DEFAULT_PT_POTO";
	Name = "POTO";
	Colour = PIXPACK(0xb08464);
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
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

	Flammable = 5;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 50;

	HeatConduct = 150;
	Description = "Potatoes.";

	DefaultProperties.life = 4;

	Properties = TYPE_PART | PROP_EDIBLE;
	FoodValue = 10;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 273.15 + 600.0f;
	HighTemperatureTransition = PT_FIRE;

	Update = &Element_POTO::update;
	Graphics = &Element_POTO::graphics;
	Create = &Element_CLST::create;
}

//#TPT-Directive ElementHeader Element_POTO static int update(UPDATE_FUNC_ARGS)
int Element_POTO::update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * pavg0 - max temp
	 * life  - water content
	 * tmp   - graphics
	 */
	if (parts[i].temp > parts[i].pavg[0])
		parts[i].pavg[0] = parts[i].temp;

	if (parts[i].temp > 100.0f + 273.15f && parts[i].life > 0) {
		for (int rx = -1; rx <= 1; ++rx)
		for (int ry = -1; ry <= 1; ++ry)
			if (BOUNDS_CHECK && (rx || ry)) {
				int r = pmap[y + ry][x + rx];
				if (!r) {
					parts[i].life--;
					sim->create_part(-1, x + rx, y + ry, PT_WTRV);
					return 0;
				}
			}
	}

	return 0;
}

//#TPT-Directive ElementHeader Element_POTO static int graphics(GRAPHICS_FUNC_ARGS)
int Element_POTO::graphics(GRAPHICS_FUNC_ARGS) {
	int z = (cpart->tmp - 5) * 8; // Speckles!
	*colr += z;
	*colg += z;
	*colb += z;

	return 0;
}

Element_POTO::~Element_POTO() {}
