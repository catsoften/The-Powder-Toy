#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_VDIV PT_VDIV 259
Element_VDIV::Element_VDIV()
{
	Identifier = "DEFAULT_PT_VDIV";
	Name = "VDIV";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_VDIV::update;
	Graphics = &Element_VDIV::graphics;
}

//#TPT-Directive ElementHeader Element_VDIV static int update(UPDATE_FUNC_ARGS)
int Element_VDIV::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_VDIV static int graphics(GRAPHICS_FUNC_ARGS)
int Element_VDIV::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_VDIV::~Element_VDIV() {}
