#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_THRS PT_THRS 211
Element_THRS::Element_THRS()
{
	Identifier = "DEFAULT_PT_THRS";
	Name = "THRS";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPACE;
	Enabled = 1;

	// element properties here

	Update = &Element_THRS::update;
	Graphics = &Element_THRS::graphics;
}

//#TPT-Directive ElementHeader Element_THRS static int update(UPDATE_FUNC_ARGS)
int Element_THRS::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_THRS static int graphics(GRAPHICS_FUNC_ARGS)
int Element_THRS::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_THRS::~Element_THRS() {}
