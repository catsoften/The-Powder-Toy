#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_VCMB PT_VCMB 260
Element_VCMB::Element_VCMB()
{
	Identifier = "DEFAULT_PT_VCMB";
	Name = "VCMB";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_VCMB::update;
	Graphics = &Element_VCMB::graphics;
}

//#TPT-Directive ElementHeader Element_VCMB static int update(UPDATE_FUNC_ARGS)
int Element_VCMB::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_VCMB static int graphics(GRAPHICS_FUNC_ARGS)
int Element_VCMB::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_VCMB::~Element_VCMB() {}
