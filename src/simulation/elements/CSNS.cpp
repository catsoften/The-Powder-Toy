#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_CSNS PT_CSNS 286
Element_CSNS::Element_CSNS()
{
	Identifier = "DEFAULT_PT_CSNS";
	Name = "CSNS";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_CSNS::update;
	Graphics = &Element_CSNS::graphics;
}

//#TPT-Directive ElementHeader Element_CSNS static int update(UPDATE_FUNC_ARGS)
int Element_CSNS::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_CSNS static int graphics(GRAPHICS_FUNC_ARGS)
int Element_CSNS::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_CSNS::~Element_CSNS() {}
