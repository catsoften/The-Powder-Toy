#include "simulation/ElementCommon.h"
#include "simulation/Spaceship.h"
#include "simulation/Thruster.h"
#include <climits>
#include <vector>

//#TPT-Directive ElementClass Element_COTR PT_COTR 208
Element_COTR::Element_COTR()
{
	Identifier = "DEFAULT_PT_COTR";
	Name = "COTR";
	Colour = PIXPACK(0x5c0e0e);
	MenuVisible = 1;
	MenuSection = SC_SPACE;
	Enabled = 1;


	// element properties here
	Update = &Element_COTR::update;
	Graphics = &Element_COTR::graphics;
	Create = &Element_COTR::create;
	ChangeType = &Element_COTR::changeType;
}

//#TPT-Directive ElementHeader Element_COTR static void create(ELEMENT_CREATE_FUNC_ARGS)
void Element_COTR::create(ELEMENT_CREATE_FUNC_ARGS) {
	sim->parts[i].pavg[0] = -1;
}

//#TPT-Directive ElementHeader Element_COTR static void changeType(ELEMENT_CHANGETYPE_FUNC_ARGS)
void Element_COTR::changeType(ELEMENT_CHANGETYPE_FUNC_ARGS) {
	// Remove self if dead
	if (SHIPS::ships.count(sim->parts[i].pavg[0]) > 0)
		SHIPS::ships[sim->parts[i].pavg[0]].destroy(sim); // Ship self-destructs if controller dies
}

//#TPT-Directive ElementHeader Element_COTR static int update(UPDATE_FUNC_ARGS)
int Element_COTR::update(UPDATE_FUNC_ARGS)
{
	// pavg[0] is unique ship ID
	// Generate ship ID or regenerate if it doesn't exist
	if (parts[i].pavg[0] < 0 || !SHIPS::is_ship(parts[i].pavg[0])) {
		parts[i].pavg[0] = SHIPS::create_ship(parts[i].pavg[0]);
		SHIPS::ships[parts[i].pavg[0]].add_component(i, parts[i].type);
	}

	SHIPS::move_ship(sim, parts[i].pavg[0]);
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
