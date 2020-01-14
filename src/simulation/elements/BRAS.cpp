#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_BRAS PT_BRAS 265
Element_BRAS::Element_BRAS()
{
	Identifier = "DEFAULT_PT_BRAS";
	Name = "BRAS";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_BRAS::update;
	Graphics = &Element_BRAS::graphics;
}

//#TPT-Directive ElementHeader Element_BRAS static int update(UPDATE_FUNC_ARGS)
int Element_BRAS::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_BRAS static int graphics(GRAPHICS_FUNC_ARGS)
int Element_BRAS::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_BRAS::~Element_BRAS() {}
