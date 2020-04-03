#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_TLNE PT_TLNE 317
Element_TLNE::Element_TLNE()
{
	Identifier = "DEFAULT_PT_TLNE";
	Name = "TLNE";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_TLNE::update;
	Graphics = &Element_TLNE::graphics;
}

//#TPT-Directive ElementHeader Element_TLNE static int update(UPDATE_FUNC_ARGS)
int Element_TLNE::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_TLNE static int graphics(GRAPHICS_FUNC_ARGS)
int Element_TLNE::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_TLNE::~Element_TLNE() {}
