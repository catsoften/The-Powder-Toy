#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_PHSP PT_PHSP 281
Element_PHSP::Element_PHSP()
{
	Identifier = "DEFAULT_PT_PHSP";
	Name = "PHSP";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_PHSP::update;
	Graphics = &Element_PHSP::graphics;
}

//#TPT-Directive ElementHeader Element_PHSP static int update(UPDATE_FUNC_ARGS)
int Element_PHSP::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_PHSP static int graphics(GRAPHICS_FUNC_ARGS)
int Element_PHSP::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_PHSP::~Element_PHSP() {}
