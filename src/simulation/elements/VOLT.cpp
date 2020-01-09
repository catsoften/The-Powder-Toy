#include "simulation/ElementCommon.h"
#include "simulation/magnetics/magnetics.h"

//#TPT-Directive ElementClass Element_VOLT PT_VOLT 254
Element_VOLT::Element_VOLT()
{
	Identifier = "DEFAULT_PT_VOLT";
	Name = "VOLT";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_ELECTROMAG;
	Enabled = 1;

	// element properties here
	Weight = 100;

	Update = &Element_VOLT::update;
	Graphics = &Element_VOLT::graphics;
}

//#TPT-Directive ElementHeader Element_VOLT static int update(UPDATE_FUNC_ARGS)
int Element_VOLT::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_VOLT static int graphics(GRAPHICS_FUNC_ARGS)
int Element_VOLT::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_VOLT::~Element_VOLT() {}
