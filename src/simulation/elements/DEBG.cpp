#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_DEBG PT_DEBG 272
Element_DEBG::Element_DEBG()
{
	Identifier = "DEFAULT_PT_DEBG";
	Name = "DEBG";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_DEBG::update;
	Graphics = &Element_DEBG::graphics;
}

//#TPT-Directive ElementHeader Element_DEBG static int update(UPDATE_FUNC_ARGS)
int Element_DEBG::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_DEBG static int graphics(GRAPHICS_FUNC_ARGS)
int Element_DEBG::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_DEBG::~Element_DEBG() {}
