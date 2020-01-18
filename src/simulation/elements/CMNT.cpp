#include "simulation/ElementCommon.h"
#define MAX_LIFE_CMNT 3000

//#TPT-Directive ElementClass Element_CMNT PT_CMNT 243
Element_CMNT::Element_CMNT() {
	Identifier = "DEFAULT_PT_CMNT";
	Name = "CMNT";
	Colour = PIXPACK(0xb8b8b8);
	MenuVisible = 1;
	MenuSection = SC_LIQUID;
	Enabled = 1;

	Advection = 0.2f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f  * CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;
	Weight = 70;

	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
	HeatConduct = 50;
	Description = "Cement. Slowly dries out into reinforced concrete. Makes weaker concrete with contamination.";

	Properties = TYPE_LIQUID | PROP_LIFE_DEC | PROP_NEUTPENETRATE;
	DefaultProperties.life = 2000;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 983.0f;
	HighTemperatureTransition = PT_STNE;

	Create = &Element_CLST::create;
	Update = &Element_CMNT::update;
	Graphics = &Element_CMNT::graphics;
}

//#TPT-Directive ElementHeader Element_CMNT static int update(UPDATE_FUNC_ARGS)
int Element_CMNT::update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * life - water absorbed 
	 * tmp  - Graphical speckles
	 * tmp2 - Purity, if contaminated may only produce regular CNCT
	 * 			0 = pure, 100 = total contamination
	 * 			> 120 = makes STNE, not even CNCT lol
	 */
	int rx, ry, r, rt;
	float nvx = 0.0f, nvy = 0.0f;

	// Decrement life based on temp
	if (parts[i].temp > 273.15f)
		parts[i].life -= (parts[i].temp - 273.15f) / 50.0f;

	// Change to concrete
	if (parts[i].life <= 0) {
		int type = PT_RCRT;
		if (parts[i].tmp2 > 120)
			type = PT_STNE;
		else if (parts[i].tmp2 > 50)
			type = PT_CNCT;
		parts[i].life = 0;
		parts[i].tmp = parts[i].tmp2 = 0;
		sim->part_change_type(i, x, y, type);
		return 1;
	}

	// Heat up slightly when hardening
	parts[i].temp += 0.01f;

	for (rx = -1; rx <= 1; ++rx)
		for (ry = -1; ry <= 1; ++ry)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = pmap[y + ry][x + rx];
				if (!r) continue;
				rt = TYP(r);

				// Stick to solids and concrete and powders
				if (sim->elements[rt].Properties & TYPE_SOLID ||
					sim->elements[rt].Properties & TYPE_PART) {
					nvx += rx;
					nvy += ry;
				}
				else if (rt == PT_CMNT && RNG::Ref().chance(1, 10)) {
					nvx += rx * 0.2f;
					nvy += ry * 0.2f;
				}

				// Touching water
				if (rt == PT_WATR || rt == PT_DSTW || rt == PT_SWTR || rt == PT_SLTW || rt == PT_CBNW || rt == PT_IOSL) {
					// Sugar water and salt water are impure and will make low quality concrete
					if (rt == PT_SWTR || rt == PT_SLTW || rt == PT_CBNW || rt == PT_IOSL)
						parts[i].tmp2 += 5;
					// Stores at most 3000 life per px
					if (parts[i].life > MAX_LIFE_CMNT)
						continue;

					parts[i].life += 200;
					sim->kill_part(ID(r));
					continue;
				}

				// Touching "impure" elements
				if ((rt == PT_CRBN || rt == PT_DUST || rt == PT_SALT) && RNG::Ref().chance(1, 100))
					parts[i].tmp2 += 2;
				
				// Average life between particles
				else if (rt == PT_CMNT && parts[ID(r)].life > parts[i].life) {
					int avg = (parts[ID(r)].life + parts[i].life) / 2 + 1;
					parts[ID(r)].life = parts[i].life = avg;
				}
			}

	// Stick
	if (nvx != 0.0f || nvy != 0.0f) {
		parts[i].vx = nvx;
		parts[i].vy = nvy;
	}

	return 0;
}

//#TPT-Directive ElementHeader Element_CMNT static int graphics(GRAPHICS_FUNC_ARGS)
int Element_CMNT::graphics(GRAPHICS_FUNC_ARGS) {
	*pixel_mode |= PMODE_BLUR | PMODE_BLOB;

	// 184 -> 168 depending on life, 3000 -> 0
	*colr = 184 - (184 - 168) * std::min(cpart->life / (float)MAX_LIFE_CMNT, 1.0f);
	*colg = 184 - (184 - 168) * std::min(cpart->life / (float)MAX_LIFE_CMNT, 1.0f);
	*colb = 184 - (184 - 168) * std::min(cpart->life / (float)MAX_LIFE_CMNT, 1.0f);

	int z = (cpart->tmp - 5) * 4; //speckles!
	*colr += z;
	*colg += z;
	*colb += z;

	return 0;
}

Element_CMNT::~Element_CMNT() {}

#undef MAX_LIFE_CMNT