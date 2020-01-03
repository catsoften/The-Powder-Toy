#include "simulation/ElementCommon.h"
#include "simulation/Spaceship.h"

#include <cmath>
#include <vector>


//#TPT-Directive ElementClass Element_THRS PT_THRS 211
Element_THRS::Element_THRS()
{
	Identifier = "DEFAULT_PT_THRS";
	Name = "THRS";
	Colour = PIXPACK(0x030163);
	MenuVisible = 1;
	MenuSection = SC_SPACE;
	Enabled = 1;

	// element properties here

	Update = &Element_THRS::update;
	Graphics = &Element_THRS::graphics;
	Create = &Element_HULL::create;
	ChangeType = &Element_HULL::changeType;
}

//#TPT-Directive ElementHeader Element_THRS static int update(UPDATE_FUNC_ARGS)
int Element_THRS::update(UPDATE_FUNC_ARGS) {
	// Clone pavg[0] and ship state
	Element_HULL::update(sim, i, x, y, surround_space, nt, parts, pmap);
	return 0;
}

//#TPT-Directive ElementHeader Element_THRS static int graphics(GRAPHICS_FUNC_ARGS)
int Element_THRS::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_THRS::~Element_THRS() {}
