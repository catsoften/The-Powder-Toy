#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_PAPR PT_PAPR 271
Element_PAPR::Element_PAPR()
{
	Identifier = "DEFAULT_PT_PAPR";
	Name = "PAPR";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_PAPR::update;
	Graphics = &Element_PAPR::graphics;
}

//#TPT-Directive ElementHeader Element_PAPR static int update(UPDATE_FUNC_ARGS)
int Element_PAPR::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_PAPR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_PAPR::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_PAPR::~Element_PAPR() {}
