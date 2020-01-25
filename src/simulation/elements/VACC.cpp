#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_VACC PT_VACC 314
Element_VACC::Element_VACC()
{
	Identifier = "DEFAULT_PT_VACC";
	Name = "VACC";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_VACC::update;
	Graphics = &Element_VACC::graphics;
}

//#TPT-Directive ElementHeader Element_VACC static int update(UPDATE_FUNC_ARGS)
int Element_VACC::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_VACC static int graphics(GRAPHICS_FUNC_ARGS)
int Element_VACC::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_VACC::~Element_VACC() {}
