#include "simulation/ElementCommon.h"
#include "simulation/magnetics/magnetics.h"

//#TPT-Directive ElementClass Element_RSPK PT_RSPK 253
Element_RSPK::Element_RSPK()
{
	Identifier = "DEFAULT_PT_RSPK";
	Name = "RSPK";
	Colour = PIXPACK(0xFFFF80);
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
	HotAir = 0.001f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;
	PhotonReflectWavelengths = 0x00000000;

	Weight = -1;

	HeatConduct = 0;
	Description = "More realistic spark.";

	Properties = TYPE_ENERGY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_RSPK::update;
	Graphics = &Element_RSPK::graphics;
}

//#TPT-Directive ElementHeader Element_RSPK static int update(UPDATE_FUNC_ARGS)
int Element_RSPK::update(UPDATE_FUNC_ARGS) {
	/**
	 * pavg0  - Current charge at location
	 * pavg1  - Current voltage
	 * 
	 * RSPK makes an electric field in the direction it goes
	 */

	// Temp code, move to create
	if (!parts[i].flags) {
		parts[i].flags = 1;
		parts[i].pavg[0] = 100;
	}

	if (parts[i].tmp == 1)
		sim->emfield->electric[FASTXY(x / EMCELL, y /EMCELL)] += 260.5f;
	else if (parts[i].tmp == 2)
		sim->emfield->electric[FASTXY(x / EMCELL, y /EMCELL)] -= 260.5f;

	// Negate velocity, or it can be affected by Newtonian gravity
	parts[i].vx = parts[i].vy = 0;

	// Make sure currently on a conductable element, otherwise self
	// destruct. (TODO: Check dielectric constant of air)
	int r = pmap[y][x];
	if (parts[i].tmp == 0 && (!r || !(sim->elements[TYP(r)].Properties & PROP_CONDUCTS))) {
		sim->kill_part(i);
		return 1;
	}

	parts[i].pavg[1] = sim->emfield->electric[FASTXY(x / EMCELL, y / EMCELL)];

	sim->emfield->electric[FASTXY(x / EMCELL, y / EMCELL)] -= parts[i].pavg[0] / 500.0f;

	if (TYP(r) == PT_GOLD)
		parts[i].pavg[0] = 2000;
	if (TYP(r) == PT_TTAN)
		parts[i].pavg[0] = -2000;

	int rx, ry;
	float ex = sim->emfield->ex[FASTXY(x / EMCELL, y /EMCELL)];
	float ey = sim->emfield->ey[FASTXY(x / EMCELL, y /EMCELL)];

	parts[i].life = isign(ex);
	parts[i].tmp2 = isign(ey);

	for (rx = -1; rx <= 1; ++rx)
		for (ry = -1; ry <= 1; ++ry)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = sim->photons[y + ry][x + rx];
				if (!r) continue;
				if (TYP(r) == PT_RSPK) {
					float avg = (parts[ID(r)].pavg[0] + parts[i].pavg[0]) / 2;
					parts[ID(r)].pavg[0] = parts[i].pavg[0] = avg;
				}	
			}

	// r = sim->photons[y + isign(ey)][x + isign(ex)];
	// if (r && TYP(r) == PT_RSPK) {
	// 	parts[ID(r)].pavg[0] += parts[i].pavg[0] / 2;
	// 	parts[i].pavg[0] *= 0.5f;
	// }
	// else {
	// 	for (rx = -1; rx <= 1; ++rx)
	// 	for (ry = -1; ry <= 1; ++ry)
	// 		if (BOUNDS_CHECK && (rx || ry) && (rx == isign(ex) || ry == isign(ey))) {
	// 			r = sim->photons[y + ry][x + rx];
	// 			if (!r) continue;
	// 			if (TYP(r) == PT_RSPK) {
	// 				parts[ID(r)].pavg[0] += parts[i].pavg[0] / 2;
	// 				parts[i].pavg[0] *= 0.5f;
	// 			}	
	// 		}
	// }
	return 0;
}

//#TPT-Directive ElementHeader Element_RSPK static int graphics(GRAPHICS_FUNC_ARGS)
int Element_RSPK::graphics(GRAPHICS_FUNC_ARGS) {
	*firea = 30;
	*firer = *colr / 2;
	*fireg = *colg / 2;
	*fireb = *colb / 2;
	*pixel_mode |= FIRE_SPARK;

	if (cpart->pavg[0] > 255)
		*colr = 0, *colg = 255, *colb = 0;
	else if (cpart->pavg[0] < -255)
		*colr = 255, *colg = 0, *colb = 0;
	return 0;
}

Element_RSPK::~Element_RSPK() {}
