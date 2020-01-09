#include "simulation/ElementCommon.h"
#include "simulation/Spaceship.h"

static int graphics(GRAPHICS_FUNC_ARGS);

void Element_HULL_create(ELEMENT_CREATE_FUNC_ARGS);
int Element_HULL_update(UPDATE_FUNC_ARGS);
void Element_HULL_changeType(ELEMENT_CHANGETYPE_FUNC_ARGS);

void Element::Element_HULL()
{
	Identifier = "DEFAULT_PT_HULL";
	Name = "HULL";
	Colour = PIXPACK(0x77777a);
	MenuVisible = 1;
	MenuSection = SC_SPACE;
	Enabled = 1;

	// element properties here

	Update = &Element_HULL_update;
	Graphics = &graphics;
	Create = &Element_HULL_create;
	ChangeType = &Element_HULL_changeType;
}

void Element_HULL_create(ELEMENT_CREATE_FUNC_ARGS) {
	sim->parts[i].pavg[0] = -1;
}

void Element_HULL_changeType(ELEMENT_CHANGETYPE_FUNC_ARGS) {
	if (SHIPS::is_ship(sim->parts[i].pavg[0]))
		SHIPS::ships[sim->parts[i].pavg[0]].remove_component(i, sim->parts[i].type);
}

int Element_HULL_update(UPDATE_FUNC_ARGS) {
	// Reset pavg0 if ship disappears
	if (!SHIPS::is_ship(parts[i].pavg[0]))
		parts[i].pavg[0] = -1;

	// Copy pavg[0] from nearby components
	if (parts[i].pavg[0] < 0)
		SHIPS::clonePAVG(sim, i, x, y);

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}



