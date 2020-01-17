#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_FERO PT_FERO 279
Element_FERO::Element_FERO()
{
	Identifier = "DEFAULT_PT_FERO";
	Name = "FERO";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_FERO::update;
	Graphics = &Element_FERO::graphics;
}

//#TPT-Directive ElementHeader Element_FERO static int update(UPDATE_FUNC_ARGS)
int Element_FERO::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_FERO static int graphics(GRAPHICS_FUNC_ARGS)
int Element_FERO::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_FERO::~Element_FERO() {}
