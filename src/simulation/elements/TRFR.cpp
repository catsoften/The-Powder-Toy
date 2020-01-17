#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_TRFR PT_TRFR 278
Element_TRFR::Element_TRFR()
{
	Identifier = "DEFAULT_PT_TRFR";
	Name = "TRFR";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_TRFR::update;
	Graphics = &Element_TRFR::graphics;
}

//#TPT-Directive ElementHeader Element_TRFR static int update(UPDATE_FUNC_ARGS)
int Element_TRFR::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_TRFR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_TRFR::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_TRFR::~Element_TRFR() {}
