#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_BRKN PT_BRKN 275
Element_BRKN::Element_BRKN()
{
	Identifier = "DEFAULT_PT_BRKN";
	Name = "BRKN";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_BRKN::update;
	Graphics = &Element_BRKN::graphics;
}

//#TPT-Directive ElementHeader Element_BRKN static int update(UPDATE_FUNC_ARGS)
int Element_BRKN::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_BRKN static int graphics(GRAPHICS_FUNC_ARGS)
int Element_BRKN::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_BRKN::~Element_BRKN() {}
