#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_TRST PT_TRST 315
Element_TRST::Element_TRST()
{
	Identifier = "DEFAULT_PT_TRST";
	Name = "TRST";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_TRST::update;
	Graphics = &Element_TRST::graphics;
}

//#TPT-Directive ElementHeader Element_TRST static int update(UPDATE_FUNC_ARGS)
int Element_TRST::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_TRST static int graphics(GRAPHICS_FUNC_ARGS)
int Element_TRST::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_TRST::~Element_TRST() {}
