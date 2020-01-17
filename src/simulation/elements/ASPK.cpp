#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_ASPK PT_ASPK 283
Element_ASPK::Element_ASPK()
{
	Identifier = "DEFAULT_PT_ASPK";
	Name = "ASPK";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_ASPK::update;
	Graphics = &Element_ASPK::graphics;
}

//#TPT-Directive ElementHeader Element_ASPK static int update(UPDATE_FUNC_ARGS)
int Element_ASPK::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_ASPK static int graphics(GRAPHICS_FUNC_ARGS)
int Element_ASPK::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_ASPK::~Element_ASPK() {}
