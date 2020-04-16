#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_LOGC PT_LOGC 316
Element_LOGC::Element_LOGC() {
	Identifier = "DEFAULT_PT_LOGC";
	Name = "LOGC";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_ELECTROMAG;
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
	Description = "Multi-pixel Logic gate. Use PSCN / COPR for input, NSCN / ZINC for output. Tmp = type, pavg0 = output voltage.";
	DefaultProperties.pavg[0] = 5.0f;

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 2300.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_LOGC::update;
	Graphics = &Element_LOGC::graphics;
}

//#TPT-Directive ElementHeader Element_LOGC static int update(UPDATE_FUNC_ARGS)
int Element_LOGC::update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * tmp - Type
	 * 
	 * tmp2 - Positive inputs in this frame
	 * life - Negative inputs in this frame
	 * pavg0 - Output voltage
	 * pavg1 - Effective output voltage
	 */
	int output_id = -1, ox, oy;
	for (int rx = -1; rx <= 1; rx++)
	for (int ry = -1; ry <= 1; ry++)
		if (BOUNDS_CHECK && (rx || ry)) {
			int r = pmap[y + ry][x + rx];
			if (!r) continue;

			if (TYP(r) == PT_INWR) {

			}
			
		}

	return 0;
}

//#TPT-Directive ElementHeader Element_LOGC static int graphics(GRAPHICS_FUNC_ARGS)
int Element_LOGC::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here

	return 0;
}

Element_LOGC::~Element_LOGC() {}
