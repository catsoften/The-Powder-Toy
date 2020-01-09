#include "simulation/ElementCommon.h"
#include "simulation/magnetics/magnetics.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_MGNT()
{
	Identifier = "DEFAULT_PT_MGNT";
	Name = "MGNT";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_ELECTROMAG;
	Enabled = 1;
	Weight = 100;

	Description = "Permanent magnet.";
	

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS) {
	if (!parts[i].flags) {
		parts[i].flags= 1;
		sim->emfield->change_magnetic[FASTXY(x / EMCELL, y / EMCELL)] = 64.0f;
		sim->emfield->changed_this_frame = true;
	}

	//if (RNG::Ref().chance(1, 200))
	// if (sim->timer % 200 == 0)
	// sim->emfield->EditField(x, y, RNG::Ref().uniform01() * 128.0f - 64.0f);

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}



