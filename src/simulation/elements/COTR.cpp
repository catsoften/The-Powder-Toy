#include "simulation/ElementCommon.h"
#include "simulation/Spaceship.h"
#include "common/tpt-rand.h"
//#include <bits/stdc++.h>
#include <climits>
#include <vector>

//#TPT-Directive ElementClass Element_COTR PT_COTR 208
Element_COTR::Element_COTR()
{
	Identifier = "DEFAULT_PT_COTR";
	Name = "COTR";
	Colour = PIXPACK(0xAAAAAA);
	MenuVisible = 1;
	MenuSection = SC_SPACE;
	Enabled = 1;

	// element properties here
	Update = &Element_COTR::update;
	Graphics = &Element_COTR::graphics;
	Create = &Element_COTR::create;
}

//#TPT-Directive ElementHeader Element_COTR static void create(ELEMENT_CREATE_FUNC_ARGS)
void Element_COTR::create(ELEMENT_CREATE_FUNC_ARGS) {
	SHIPS::createShip(i);
}

//#TPT-Directive ElementHeader Element_COTR static int update(UPDATE_FUNC_ARGS)
int Element_COTR::update(UPDATE_FUNC_ARGS)
{
	// update code here
	SHIPS::translate(sim, i);
	return 0;
}

//#TPT-Directive ElementHeader Element_COTR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_COTR::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_COTR::~Element_COTR() {}
