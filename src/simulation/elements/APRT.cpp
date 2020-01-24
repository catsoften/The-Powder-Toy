#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_APRT PT_APRT 306
Element_APRT::Element_APRT()
{
	Identifier = "DEFAULT_PT_APRT";
	Name = "APRT";
	Colour = PIXPACK(0x000099);
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = -.99f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = -1;

	HeatConduct = 61;
	Description = "Anti-Protons. Transfer heat to materials, and removes sparks.";

	Properties = TYPE_ENERGY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	DefaultProperties.life = 75;

	Update = &Element_APRT::update;
	Graphics = &Element_PROT::graphics;
	Create = &Element_PROT::create;
}

//#TPT-Directive ElementHeader Element_APRT static int update(UPDATE_FUNC_ARGS)
int Element_APRT::update(UPDATE_FUNC_ARGS) {
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_APRT static int graphics(GRAPHICS_FUNC_ARGS)
int Element_APRT::graphics(GRAPHICS_FUNC_ARGS) {
	*firea = 7;
	*firer = 170;
	*fireg = 170;
	*fireb = 250;

	*pixel_mode |= FIRE_BLEND;
	return 1;
}

Element_APRT::~Element_APRT() {}
