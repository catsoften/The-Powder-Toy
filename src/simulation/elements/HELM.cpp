#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_HELM PT_HELM 287
Element_HELM::Element_HELM()
{
	Identifier = "DEFAULT_PT_HELM";
	Name = "HELM";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	// element properties here

	Update = &Element_HELM::update;
	Graphics = &Element_HELM::graphics;
}

//#TPT-Directive ElementHeader Element_HELM static int update(UPDATE_FUNC_ARGS)
int Element_HELM::update(UPDATE_FUNC_ARGS)
{
	// update code here

	return 0;
}

//#TPT-Directive ElementHeader Element_HELM static int graphics(GRAPHICS_FUNC_ARGS)
int Element_HELM::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_HELM::~Element_HELM() {}
