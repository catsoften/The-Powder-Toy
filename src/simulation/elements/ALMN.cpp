#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_ALMN PT_ALMN 282
Element_ALMN::Element_ALMN()
{
	Identifier = "DEFAULT_PT_ALMN";
	Name = "ALMN";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_ALMN::update;
	Graphics = &Element_ALMN::graphics;
}

//#TPT-Directive ElementHeader Element_ALMN static int update(UPDATE_FUNC_ARGS)
int Element_ALMN::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_ALMN static int graphics(GRAPHICS_FUNC_ARGS)
int Element_ALMN::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_ALMN::~Element_ALMN() {}
