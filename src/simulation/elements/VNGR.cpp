#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_VNGR PT_VNGR 308
Element_VNGR::Element_VNGR()
{
	Identifier = "DEFAULT_PT_VNGR";
	Name = "VNGR";
	Colour = PIXPACK(0xf0f6ff);
	MenuVisible = 1;
	MenuSection = SC_LIQUID;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 30;

	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
	HeatConduct = 29;
	Description = "Vinegar.";

	Properties = TYPE_LIQUID | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = 273.15f + 16.6f;
	LowTemperatureTransition = PT_ICEI;
	HighTemperature = 273.15f + 118.1f;
	HighTemperatureTransition = PT_WTRV;

	Update = &Element_VNGR::update;
	Graphics = &Element_VNGR::graphics;
}

//#TPT-Directive ElementHeader Element_VNGR static int update(UPDATE_FUNC_ARGS)
int Element_VNGR::update(UPDATE_FUNC_ARGS) {
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_VNGR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_VNGR::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_VNGR::~Element_VNGR() {}
