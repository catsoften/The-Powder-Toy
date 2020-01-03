#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_RADR PT_RADR 209
Element_RADR::Element_RADR()
{
	Identifier = "DEFAULT_PT_RADR";
	Name = "RADR";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPACE;
	Enabled = 1;

	// element properties here

	Update = &Element_RADR::update;
	Graphics = &Element_RADR::graphics;
}

//#TPT-Directive ElementHeader Element_RADR static int update(UPDATE_FUNC_ARGS)
int Element_RADR::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_RADR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_RADR::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_RADR::~Element_RADR() {}
