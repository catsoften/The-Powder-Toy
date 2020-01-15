#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_SHRD PT_SHRD 274
Element_SHRD::Element_SHRD()
{
	Identifier = "DEFAULT_PT_SHRD";
	Name = "SHRD";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_SHRD::update;
	Graphics = &Element_SHRD::graphics;
}

//#TPT-Directive ElementHeader Element_SHRD static int update(UPDATE_FUNC_ARGS)
int Element_SHRD::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_SHRD static int graphics(GRAPHICS_FUNC_ARGS)
int Element_SHRD::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_SHRD::~Element_SHRD() {}
