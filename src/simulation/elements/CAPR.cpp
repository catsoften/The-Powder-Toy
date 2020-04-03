#include "simulation/ElementCommon.h"
#include "simulation/circuits/circuits.h"
#include "simulation/circuits/framework.h"

//#TPT-Directive ElementClass Element_CAPR PT_CAPR 263
Element_CAPR::Element_CAPR() {
	Identifier = "DEFAULT_PT_CAPR";
	Name = "CPTR";
	Colour = PIXPACK(0x82a5cf);
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
	Description = "Polarized electrolytic capacitor. Stores and releases charge. pavg0 = capacitance.";

	Properties = TYPE_SOLID;
	DefaultProperties.pavg[0] = 0.1f;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1273.0f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_CAPR::update;
	Graphics = NULL;
}

//#TPT-Directive ElementHeader Element_CAPR static int update(UPDATE_FUNC_ARGS)
int Element_CAPR::update(UPDATE_FUNC_ARGS) {
	/**
	 * Variables:
	 * tmp   - SPRK stored
	 * tmp2  - Flag to explode
	 * pavg0 - Capacitance (in F)
	 * pavg1 - Effective voltage
	 */
	int r;

	// CAPR can create circuit if not currently part of one
	if (parts[i].pavg[1])
		CIRCUITS::addCircuit(x, y, sim);

	// Capacitance must be positive
	parts[i].pavg[0] = fabs(parts[i].pavg[0]);

	// Explode if wrong polarity
	if (parts[i].tmp2 == 0 && sim->photons[y][x]) {
		float self_volt = parts[ID(sim->photons[y][x])].pavg[0];
		float self_current = parts[ID(sim->photons[y][x])].pavg[1];

		// This is actually a limiter from causing high current creating NaN voltages
		if (fabs(self_current) > MAX_CAPACITOR_CURRENT)
			goto explode;

		for (int rx = -1; rx <= 1; rx++)
		for (int ry = -1; ry <= 1; ry++)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = pmap[y + ry][x + rx];
				if (!r || !sim->photons[y + ry][x + rx]) continue;
				if (!positive_terminal(TYP(r))) continue;

				// Current flows from positive into CPTR if correct polarity
				// Thus other_v should >= self_volt
				float other_v = parts[ID(sim->photons[y + ry][x + rx])].pavg[0];
				if (other_v > self_volt && fabs(self_current) > MAX_CAPACITOR_REVERSE_CURRENT)
					goto explode;
			}
		if (false) {
			explode:;
			PropertyValue value;
			value.Integer = 1;
			sim->flood_prop(x, y, offsetof(Particle, tmp2), value, StructProperty::Integer);
		}
	}

	if (parts[i].tmp2) {
		if (parts[i].pavg[0] < 0.01) { // Small capacitor, sizzle
			sim->part_change_type(i, x, y, PT_SMKE);
			sim->pv[y / CELL][x / CELL] += 0.05f;
			parts[i].temp += 15.0f;
			parts[i].life = RNG::Ref().between(10, 100);
		}
		else if (parts[i].pavg[0] < 10) { // Bigger capacitor, explode into BRMT and steam
			sim->part_change_type(i, x, y, RNG::Ref().chance(1, 2) ? PT_BRMT : PT_WTRV);
			sim->pv[y / CELL][x / CELL] += 0.5f;
			parts[i].temp += 100.0f;
		}
		else if (parts[i].pavg[0] < 100) { // Really big capacitor... it's going kaboom
			sim->part_change_type(i, x, y, RNG::Ref().chance(1, 2) ? PT_FIRE : PT_BRMT);
			sim->pv[y / CELL][x / CELL] += 1.5f;
			parts[i].temp += 1000.0f;
			parts[i].life = RNG::Ref().between(100, 300);
		}
		else { // Your capacitance value is way too large
			sim->part_change_type(i, x, y, PT_EXOT);
			sim->pv[y / CELL][x / CELL] += 5.0f;
			parts[i].temp += 5000.0f;
			parts[i].life = RNG::Ref().between(100, 300);
		}
		return 0;
	}

	// Capacitor function for realistic circuits
	r = sim->photons[y][x];
	if (r && TYP(r) == PT_RSPK) {
		/**
		 * Capacitor equation:
		 * i = C dV / dt, or integral of i dt / C = V
		 */
		float current = parts[ID(r)].pavg[1];
		parts[i].pavg[1] -= INTEGRATION_TIMESTEP / parts[i].pavg[0] * current;
	}
	// Capacitor function for SPRK
	else {
		for (int rx = -1; rx <= 1; rx++)
		for (int ry = -1; ry <= 1; ry++)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = sim->pmap[y + ry][x + rx];
				if (!r) continue;

				// Charging with PSCN or COPR
				if (TYP(r) == PT_SPRK && (parts[ID(r)].ctype == PT_PSCN || parts[ID(r)].ctype == PT_COPR))
					parts[i].tmp++;
				// Discharging to NSCN or ZINC
				else if ((TYP(r) == PT_ZINC || TYP(r) == PT_NSCN) && parts[ID(r)].life == 0 && parts[i].tmp > 0) {
					parts[ID(r)].life = 4;
					parts[ID(r)].ctype = TYP(r);
					sim->part_change_type(ID(r), x + rx, y + ry, PT_SPRK);
					parts[i].tmp--;
				}
				// Share charge
				else if (TYP(r) == PT_CAPR && parts[ID(r)].tmp < parts[i].tmp) {
					parts[i].tmp--;
					parts[ID(r)].tmp++;
				}
			}
	}

	return 0;
}

Element_CAPR::~Element_CAPR() {}