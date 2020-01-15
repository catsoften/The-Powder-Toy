#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_PULP PT_PULP 273
Element_PULP::Element_PULP()
{
	Identifier = "DEFAULT_PT_PULP";
	Name = "PULP";
	Colour = PIXPACK(0xEAEAEA);
	MenuVisible = 1;
	MenuSection = SC_LIQUID;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 2;

	Flammable = 50;
	Explosive = 0;
	Meltable = 0;
	Hardness = 50;

	Weight = 31;

	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
	DefaultProperties.life = 10000;

	HeatConduct = 164;
	Description = "Paper pulp. Slowly turns into paper.";
	Properties = TYPE_LIQUID | PROP_NEUTPENETRATE | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 232.8f + 273.15f; // 451 F
	HighTemperatureTransition = PT_FIRE;

	Update = &Element_PULP::update;
	Graphics = &Element_PULP::graphics;
}

//#TPT-Directive ElementHeader Element_PULP static int update(UPDATE_FUNC_ARGS)
int Element_PULP::update(UPDATE_FUNC_ARGS) {
	if (parts[i].life <= 0) {
		sim->part_change_type(i, x, y, PT_PAPR);
		parts[i].life = 0;
		return 1;
	}

	int life_dec_bonus = std::max(0, (int)((parts[i].temp - 273.15f) / 10.0f));
	parts[i].life -= life_dec_bonus;
	return 0;
}

//#TPT-Directive ElementHeader Element_PULP static int graphics(GRAPHICS_FUNC_ARGS)
int Element_PULP::graphics(GRAPHICS_FUNC_ARGS) {
	*pixel_mode |= PMODE_BLUR;
	return 1;
}

Element_PULP::~Element_PULP() {}
