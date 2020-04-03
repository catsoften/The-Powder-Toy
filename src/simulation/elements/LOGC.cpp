#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_LOGC PT_LOGC 316
Element_LOGC::Element_LOGC()
{
	Identifier = "DEFAULT_PT_LOGC";
	Name = "LOGC";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_LOGC::update;
	Graphics = &Element_LOGC::graphics;
}

//#TPT-Directive ElementHeader Element_LOGC static int update(UPDATE_FUNC_ARGS)
int Element_LOGC::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_LOGC static int graphics(GRAPHICS_FUNC_ARGS)
int Element_LOGC::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_LOGC::~Element_LOGC() {}
