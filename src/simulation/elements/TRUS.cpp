#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_TRUS PT_TRUS 305
Element_TRUS::Element_TRUS()
{
	Identifier = "DEFAULT_PT_TRUS";
	Name = "TRUS";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_TRUS::update;
	Graphics = &Element_TRUS::graphics;
}

//#TPT-Directive ElementHeader Element_TRUS static int update(UPDATE_FUNC_ARGS)
int Element_TRUS::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_TRUS static int graphics(GRAPHICS_FUNC_ARGS)
int Element_TRUS::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_TRUS::~Element_TRUS() {}
