#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_PINV PT_PINV 313
Element_PINV::Element_PINV()
{
	Identifier = "DEFAULT_PT_PINV";
	Name = "PINV";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_PINV::update;
	Graphics = &Element_PINV::graphics;
}

//#TPT-Directive ElementHeader Element_PINV static int update(UPDATE_FUNC_ARGS)
int Element_PINV::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_PINV static int graphics(GRAPHICS_FUNC_ARGS)
int Element_PINV::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_PINV::~Element_PINV() {}
