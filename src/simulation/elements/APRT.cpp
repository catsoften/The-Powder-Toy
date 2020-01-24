#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_APRT PT_APRT 306
Element_APRT::Element_APRT()
{
	Identifier = "DEFAULT_PT_APRT";
	Name = "APRT";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_APRT::update;
	Graphics = &Element_PROT::graphics;
	Create = &Element_PROT::create;
}

//#TPT-Directive ElementHeader Element_APRT static int update(UPDATE_FUNC_ARGS)
int Element_APRT::update(UPDATE_FUNC_ARGS) {
	// update code here

	return 0;
}

Element_APRT::~Element_APRT() {}
