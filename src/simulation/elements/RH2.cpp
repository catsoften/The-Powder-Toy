#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_RH2 PT_RH2 288
Element_RH2::Element_RH2()
{
	Identifier = "DEFAULT_PT_RH2";
	Name = "RH2";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_RH2::update;
	Graphics = &Element_RH2::graphics;
}

//#TPT-Directive ElementHeader Element_RH2 static int update(UPDATE_FUNC_ARGS)
int Element_RH2::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_RH2 static int graphics(GRAPHICS_FUNC_ARGS)
int Element_RH2::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_RH2::~Element_RH2() {}
