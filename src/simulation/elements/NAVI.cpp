#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_NAVI PT_NAVI 210
Element_NAVI::Element_NAVI()
{
	Identifier = "DEFAULT_PT_NAVI";
	Name = "NAVI";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPACE;
	Enabled = 1;

	// element properties here

	Update = &Element_NAVI::update;
	Graphics = &Element_NAVI::graphics;
}

//#TPT-Directive ElementHeader Element_NAVI static int update(UPDATE_FUNC_ARGS)
int Element_NAVI::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_NAVI static int graphics(GRAPHICS_FUNC_ARGS)
int Element_NAVI::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_NAVI::~Element_NAVI() {}
