#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_FUSN PT_FUSN 312
Element_FUSN::Element_FUSN()
{
	Identifier = "DEFAULT_PT_FUSN";
	Name = "FUSN";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_FUSN::update;
	Graphics = &Element_FUSN::graphics;
}

//#TPT-Directive ElementHeader Element_FUSN static int update(UPDATE_FUNC_ARGS)
int Element_FUSN::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_FUSN static int graphics(GRAPHICS_FUNC_ARGS)
int Element_FUSN::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_FUSN::~Element_FUSN() {}
