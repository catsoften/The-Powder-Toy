#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_ERAY PT_ERAY 268
Element_ERAY::Element_ERAY()
{
	Identifier = "DEFAULT_PT_ERAY";
	Name = "ERAY";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_ERAY::update;
	Graphics = &Element_ERAY::graphics;
}

//#TPT-Directive ElementHeader Element_ERAY static int update(UPDATE_FUNC_ARGS)
int Element_ERAY::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_ERAY static int graphics(GRAPHICS_FUNC_ARGS)
int Element_ERAY::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_ERAY::~Element_ERAY() {}
