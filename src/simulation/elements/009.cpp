#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_009 PT_009 280
Element_009::Element_009()
{
	Identifier = "DEFAULT_PT_009";
	Name = "009";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_009::update;
	Graphics = &Element_009::graphics;
}

//#TPT-Directive ElementHeader Element_009 static int update(UPDATE_FUNC_ARGS)
int Element_009::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_009 static int graphics(GRAPHICS_FUNC_ARGS)
int Element_009::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_009::~Element_009() {}
