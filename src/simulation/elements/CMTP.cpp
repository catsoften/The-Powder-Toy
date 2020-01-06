#include "simulation/ElementCommon.h"

//#TPT-Directive ElementClass Element_CMTP PT_CMTP 244
Element_CMTP::Element_CMTP() {
	Identifier = "DEFAULT_PT_CMTP";
	Name = "CMTP";
	Colour = PIXPACK(0x707070);
	MenuVisible = 1;
	MenuSection = SC_POWDERS;
	Enabled = 1;

	Advection = 0.4f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.3f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 5;
	Hardness = 1;
	Weight = 90;

	HeatConduct = 150;
	Description = "Cement Powder. Mix with water to make cement.";

	Properties = TYPE_PART | PROP_DEADLY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 983.0f;
	HighTemperatureTransition = PT_LAVA;

	Create = &Element_CLST::create;
	Update = &Element_CMTP::update;
	Graphics = &Element_CMTP::graphics;
}

//#TPT-Directive ElementHeader Element_CMTP static int update(UPDATE_FUNC_ARGS)
int Element_CMTP::update(UPDATE_FUNC_ARGS) {
	int rx, ry, r, rt;
	for (rx = -1; rx <= 1; ++rx)
		for (ry = -1; ry <= 1; ++ry)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = pmap[y + ry][x + rx];
				if (!r) continue;
				rt = TYP(r);

				// Touching water
				if (rt == PT_WATR || rt == PT_DSTW || rt == PT_SWTR || rt == PT_SLTW) {
					sim->part_change_type(i, x, y, PT_CMNT);

					// Sugar water and salt water are impure and will make low quality concrete
					if (rt == PT_SWTR || rt == PT_SLTW)
						parts[i].tmp2 = 100;
					parts[i].life += 500;
					return 1;
				}
			}

	return 0;
}

//#TPT-Directive ElementHeader Element_CMTP static int graphics(GRAPHICS_FUNC_ARGS)
int Element_CMTP::graphics(GRAPHICS_FUNC_ARGS) {
	int z = (cpart->tmp - 5) * 4; //speckles!
	*colr += z;
	*colg += z;
	*colb += z;

	return 0;
}

Element_CMTP::~Element_CMTP() {}
