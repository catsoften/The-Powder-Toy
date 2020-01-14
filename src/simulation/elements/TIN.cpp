#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_TIN PT_TIN 269
Element_TIN::Element_TIN()
{
	Identifier = "DEFAULT_PT_TIN";
	Name = "TIN";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_TIN::update;
	Graphics = &Element_TIN::graphics;
}

//#TPT-Directive ElementHeader Element_TIN static int update(UPDATE_FUNC_ARGS)
int Element_TIN::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_TIN static int graphics(GRAPHICS_FUNC_ARGS)
int Element_TIN::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_TIN::~Element_TIN() {}
