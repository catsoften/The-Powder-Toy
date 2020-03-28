#include "simulation/ElementCommon.h"
#include "simulation/magnetics/magnetics.h"
#include <queue>
#include <iostream>

namespace RSPK {
const int REFRESH_EVERY_FRAMES = 5;

float get_resistance(int type, Particle *parts, int i, Simulation *sim) {
	return 0.5f; // Default conductor resistance
}

float get_power(int x, int y, Simulation *sim) {
	int r = sim->photons[y][x];
	float voltage = sim->parts[ID(r)].pavg[1];
	r = sim->pmap[y][x];
	float resistance = get_resistance(TYP(r), sim->parts, ID(r), sim);
	if (resistance == 0.0f)
		return 0.0f;
	return voltage / resistance * voltage;
}
};

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
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;
	PhotonReflectWavelengths = 0x00000000;

	Weight = -1;

	HeatConduct = 0;
	Description = "Instant spark with voltage.";

	Properties = TYPE_ENERGY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	DefaultProperties.life = 10;

	Update = &Element_RSPK::update;
	Graphics = &Element_RSPK::graphics;
	ChangeType = &Element_RSPK::changeType;
}

//#TPT-Directive ElementHeader Element_RSPK static void changeType(ELEMENT_CHANGETYPE_FUNC_ARGS)
void Element_RSPK::changeType(ELEMENT_CHANGETYPE_FUNC_ARGS) {

}

//#TPT-Directive ElementHeader Element_RSPK static int update(UPDATE_FUNC_ARGS)
int Element_RSPK::update(UPDATE_FUNC_ARGS) {
	/**
	 * tmp   - Current node ID
	 * tmp2  - 
	 * pavg0 - Voltage relative to ground
	 * pavg1 - Current through pixel
	 * dcolour - Resistance of the value its on * 10000
	 * 
	 * RSPK makes an electric field in the direction it goes
	 */
	float res = RSPK::get_resistance(parts[i].ctype, parts, ID(pmap[y][x]), sim);

	// Negate velocity, or it can be affected by Newtonian gravity
	parts[i].vx = parts[i].vy = 0;

	// Set ctype and temp to whats under it
	parts[i].ctype = TYP(pmap[y][x]);
	parts[i].temp = parts[ID(pmap[y][x])].temp;
	parts[i].dcolour = 10000 * res;

	if (!parts[i].pavg[0] && !parts[i].pavg[1]) {
		for (int rx = -1; rx <= 1; rx++)
		for (int ry = -1; ry <= 1; ry++)
			if (rx || ry) {
				int r = sim->photons[y + ry][x + rx];
				if (r && TYP(r) == PT_RSPK && parts[ID(r)].tmp) {
					parts[i].pavg[0] = parts[ID(r)].pavg[0];
					parts[i].pavg[1] = parts[ID(r)].pavg[1];
					goto end;
				}
			}
		for (int rx = -1; rx <= 1; rx++)
		for (int ry = -1; ry <= 1; ry++)
			if (rx || ry) {
				int r = sim->photons[y + ry][x + rx];
				if (r && TYP(r) == PT_RSPK && (sim->parts[ID(r)].pavg[0] || sim->parts[ID(r)].pavg[1])) {
					parts[i].pavg[0] = parts[ID(r)].pavg[0];
					parts[i].pavg[1] = parts[ID(r)].pavg[1];
					goto end;
				}
			}
		end:;
	}


	// Kill on low life, life decrements below 0 if no longer connected to a voltage source
	// that regenerates life
	// Also kill self if no voltage drop (no current)
	// Also kill self if no longer on a conductor and not a source (tmp = 1)
	// parts[i].life--;


	// Heat up the conductor its on
	int r = pmap[y][x];
	float power = RSPK::get_power(x, y, sim);
	// parts[ID(r)].temp += power / 200.0f;

	// Make sure self conductor can't be SPRKed
	// TODO this breaks SWCH
	// parts[ID(r)].life = 10;

	// Project electric field
	float efield = res == 0.0f ? 0.0f : isign(parts[i].pavg[0]) * parts[i].pavg[1] / res;
	sim->emfield->electric[FASTXY(x / EMCELL, y / EMCELL)] += efield;

	// Due to float point precision issues voltage drops are 0 when high voltages are present
	// So the solution: we superheat metals when resistance is non-zero
	// Electric field also doesn't exist, so we just set it to 2560
	if (res != 0.0f && parts[i].pavg[0] > 1000000) {
		parts[ID(pmap[y][x])].temp += 9000.0f;
		sim->emfield->electric[FASTXY(x / EMCELL, y / EMCELL)] += isign(parts[i].pavg[0]) * 2560.0f;
	}

	if (parts[i].ctype != PT_VOLT) {
		for (int rx = -1; rx <= 1; ++rx)
		for (int ry = -1; ry <= 1; ++ry)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = pmap[y + ry][x + rx];
				if (!r && RNG::Ref().chance(1, 100)) {
					// St elmo's fire
					if (parts[i].pavg[0] > 100000) {
						int ni = sim->create_part(-1, x + rx ,y + ry, PT_PLSM);
						parts[ni].temp = parts[i].temp;
						parts[ni].life = RNG::Ref().between(0, 70);
					}
					// Thermonic emission
					if (parts[i].pavg[0] > 1000) {
						int ni = sim->create_part(-3, x + rx, y + ry, PT_ELEC);
						parts[ni].temp = parts[i].temp;
						parts[ni].life = RNG::Ref().between(0, 570);
						parts[ni].vx = rx;
						parts[ni].vy = ry;
					}
				}
				// Ionizing gases
				if (parts[i].pavg[0] > 1000 && sim->elements[TYP(r)].Properties & TYPE_GAS) {
					sim->part_change_type(ID(r), x + rx, y + ry, PT_PLSM);
					parts[ID(r)].life = RNG::Ref().between(0, 570);
				}
			}
	}

	// Merge self with any RSPK occupying same space
	while (true) {
		r = sim->photons[y][x];
		if (r && ID(r) != i && TYP(r) == PT_RSPK) {
			parts[i].pavg[0] += parts[ID(r)].pavg[0];
			parts[i].pavg[1] += parts[ID(r)].pavg[1];
			sim->kill_part(ID(r));
		}
		else {
			break;
		}
	}

	return 0;
}

//#TPT-Directive ElementHeader Element_RSPK static int graphics(GRAPHICS_FUNC_ARGS)
int Element_RSPK::graphics(GRAPHICS_FUNC_ARGS) {
	*colr = 255; *colg = *colb = 0;
	*cola = 255;
	if (cpart->tmp == 0)
		*cola = *colr = *colg = *colb = 100;

	if (cpart->tmp > 1) {
		*colr = 0;
		*colg = 0;
		*colb = 255;
		//ren->fillcircle(cpart->x - 6, cpart->y - 6, 5, 5, 255, 255, 0, 55);
		//ren->drawtext(cpart->x, cpart->y - 10, String::Build(cpart->pavg[1]), 0,0,0, 255);
	}

	return 0;

	// Power = V^2 / R and is used for brightness
	float power = RSPK::get_power(nx, ny, ren->sim);
	*firea = clamp_flt(power / 10.0f, 0, 500);

	*firer = *colr / 2;
	*fireg = *colg / 2;
	*fireb = *colb / 2;
	*pixel_mode |= FIRE_SPARK | PMODE_NONE;

	if (cpart->ctype == PT_NEON) {
		int r = ren->sim->pmap[ny][nx];
		*colr = PIXR(ren->sim->parts[ID(r)].dcolour);
		*colg = PIXG(ren->sim->parts[ID(r)].dcolour);
		*colb = PIXB(ren->sim->parts[ID(r)].dcolour);
	}
	else if (*firea < 20 && cpart->ctype > 0 && cpart->ctype < PT_NUM) {
		int o_r = PIXR(ren->sim->elements[cpart->ctype].Colour);
		int o_g = PIXG(ren->sim->elements[cpart->ctype].Colour);
		int o_b = PIXB(ren->sim->elements[cpart->ctype].Colour);

		*colr = (*colr - o_r) * (*firea + 1) / 20.0f + o_r;
		*colg = (*colg - o_g) * (*firea + 1) / 20.0f + o_g;
		*colb = (*colb - o_b) * (*firea + 1) / 20.0f + o_b;
	}
	return 0;
}

Element_RSPK::~Element_RSPK() {}
