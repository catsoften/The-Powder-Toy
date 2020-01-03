#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_NICH PT_NICH 194
Element_NICH::Element_NICH()
{
	Identifier = "DEFAULT_PT_NICH";
	Name = "NICH";
	Colour = PIXPACK(0xD0D0DA);
	MenuVisible = 1;
	MenuSection = SC_ELEC;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 1;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "Nichrome wire. Conducts SPRK 1 px at a time.";

	Properties = TYPE_SOLID|PROP_CONDUCTS|PROP_LIFE_DEC|PROP_HOT_GLOW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1273.0f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_NICH::update;
	Graphics = &Element_NICH::graphics;
}

//#TPT-Directive ElementHeader Element_NICH static int update(UPDATE_FUNC_ARGS)
int Element_NICH::update(UPDATE_FUNC_ARGS) { return 1; }

//#TPT-Directive ElementHeader Element_NICH static int graphics(GRAPHICS_FUNC_ARGS)
int Element_NICH::graphics(GRAPHICS_FUNC_ARGS) { return 1; }

Element_NICH::~Element_NICH() {}
