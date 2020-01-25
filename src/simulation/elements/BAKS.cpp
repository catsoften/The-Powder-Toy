#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_BAKS PT_BAKS 309
Element_BAKS::Element_BAKS()
{
	Identifier = "DEFAULT_PT_BAKS";
	Name = "BAKS";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_BAKS::update;
	Graphics = &Element_BAKS::graphics;
}

//#TPT-Directive ElementHeader Element_BAKS static int update(UPDATE_FUNC_ARGS)
int Element_BAKS::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_BAKS static int graphics(GRAPHICS_FUNC_ARGS)
int Element_BAKS::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_BAKS::~Element_BAKS() {}
