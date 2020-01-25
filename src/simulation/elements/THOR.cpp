#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_THOR PT_THOR 311
Element_THOR::Element_THOR()
{
	Identifier = "DEFAULT_PT_THOR";
	Name = "THOR";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_THOR::update;
	Graphics = &Element_THOR::graphics;
}

//#TPT-Directive ElementHeader Element_THOR static int update(UPDATE_FUNC_ARGS)
int Element_THOR::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_THOR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_THOR::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_THOR::~Element_THOR() {}
