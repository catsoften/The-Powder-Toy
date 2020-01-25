#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_PINV PT_PINV 313
Element_PINV::Element_PINV()
{
	Identifier = "DEFAULT_PT_PINV";
	Name = "PINV";
	Colour = PIXPACK(0x00CCCC);
	MenuVisible = 1;
	MenuSection = SC_POWERED;
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
	Meltable = 0;
	Hardness = 15;

	Weight = 100;

	HeatConduct = 164;
	Description = "Invisible to particles when powered.";

	Properties = TYPE_SOLID | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_PINV::update;
	Graphics = &Element_PINV::graphics;
}

//#TPT-Directive ElementHeader Element_PINV static int update(UPDATE_FUNC_ARGS)
int Element_PINV::update(UPDATE_FUNC_ARGS) {
	int rx, ry, r;
	for (rx = -1; rx <= 1; rx++)
	for (ry = -1; ry <= 1; ry++)
		if (BOUNDS_CHECK && (rx || ry)) {
			r = pmap[y + ry][x + rx];
			if (!r) continue;
			
			if (TYP(r) == PT_SPRK) {
				if (parts[ID(r)].ctype == PT_PSCN) {
					PropertyValue value;
					value.Integer = 10;
					sim->flood_prop(x, y, offsetof(Particle, life), value, StructProperty::Integer);
				}
				else if (parts[ID(r)].ctype == PT_NSCN) {
					PropertyValue value;
					value.Integer = 0;
					sim->flood_prop(x, y, offsetof(Particle, life), value, StructProperty::Integer);
				}
			}
		}

	return 0;
}

//#TPT-Directive ElementHeader Element_PINV static int graphics(GRAPHICS_FUNC_ARGS)
int Element_PINV::graphics(GRAPHICS_FUNC_ARGS) {
	if (cpart->life == 0) {
		*colr *= 0.3f;
		*colg *= 0.3f;
		*colb *= 0.3f;
	}

	return 0;
}

Element_PINV::~Element_PINV() {}
