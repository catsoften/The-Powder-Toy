#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

#define PIXA(x) (((x)>>24)&0xFF)

void Element::Element_CSNS() {
	Identifier = "DEFAULT_PT_CSNS";
	Name = "CSNS";
	Colour = PIXPACK(0xC242F5);
	MenuVisible = 1;
	MenuSection = SC_SENSOR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.96f;
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

	Weight = 100;

	HeatConduct = 0;
	Description = "Color sensor, creates a spark when something with its dcolor is nearby.";

	Properties = TYPE_SOLID;

	DefaultProperties.tmp2 = 2;
	DefaultProperties.temp = 4.0f + 273.15f;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * temp = difference must be within this value. If temp is negative then must be greater
	 * tmp2 = radius, 1 - 25 inclusive
	 * tmp  = 1 - ignore DECO
	 * dcolor = what to search for
	 */
	int radius = std::min(std::max(1, parts[i].tmp2), 25);
	int rx, ry, r;
	int cr, cg, cb, ct, ca; // Color of particle
	int useless; // Filler int to store unneeded parameters
	float alpha, error_allowed;
	bool found = false;

	if (!parts[i].dcolour) return 0; // No dcolor set, do nothing

	// Target color
	int tr = PIXR(parts[i].dcolour) / 255.0f * PIXA(parts[i].dcolour);
	int tg = PIXG(parts[i].dcolour) / 255.0f * PIXA(parts[i].dcolour);
	int tb = PIXB(parts[i].dcolour) / 255.0f * PIXA(parts[i].dcolour);

	for (rx = -radius; rx <= radius; rx++)
	for (ry = -radius; ry <= radius; ry++)
		if (BOUNDS_CHECK && (rx || ry)) {
			r = pmap[y + ry][x + rx];
			if (!r) r = sim->photons[y + ry][x + rx];
			if (!r || TYP(r) == PT_CSNS) continue;

			// Get color of the particle
			if (sim->elements[TYP(r)].Graphics) {
				if (sim->ren->graphicscache[TYP(r)].isready) {
					cr = sim->ren->graphicscache[TYP(r)].colr / 255.0f * sim->ren->graphicscache[TYP(r)].cola;
					cg = sim->ren->graphicscache[TYP(r)].colg / 255.0f * sim->ren->graphicscache[TYP(r)].cola;
					cb = sim->ren->graphicscache[TYP(r)].colb / 255.0f * sim->ren->graphicscache[TYP(r)].cola;
				}
				else {
					ct = sim->elements[TYP(r)].Colour;
					cr = PIXR(ct), cg = PIXG(ct), cb = PIXB(ct), ca = 255;
					(*(sim->elements[TYP(r)].Graphics))(sim->ren,
						&(sim->parts[ID(r)]), x + rx, y + ry, &useless, &ca, &cr, &cg, &cb,
							&useless, &useless, &useless, &useless);
					cr = cr / 255.0f * ca;
					cg = cg / 255.0f * ca;
					cb = cb / 255.0f * ca;
				}
			} else {
				ct = sim->elements[TYP(r)].Colour;
				cr = PIXR(ct), cg = PIXG(ct), cb = PIXB(ct);
			}

			// Blend dcolor
			if (parts[ID(r)].dcolour && parts[i].tmp == 0) {
				alpha = PIXA(parts[ID(r)].dcolour) / 255.0f;
				cr = PIXR(parts[ID(r)].dcolour) * alpha + (1 - alpha) * cr;
				cg = PIXG(parts[ID(r)].dcolour) * alpha + (1 - alpha) * cg;
				cb = PIXB(parts[ID(r)].dcolour) * alpha + (1 - alpha) * cb;
			}

			cr = std::min(255, std::max(cr, 0));
			cg = std::min(255, std::max(cg, 0));
			cb = std::min(255, std::max(cb, 0));

			// Check if close enough
			error_allowed = parts[i].temp - 273.15f;
			found = error_allowed > 0.0f ?
				fabs(cr - tr) <= error_allowed && fabs(cg - tg) <= error_allowed && fabs(cb - tb) <= error_allowed :
				fabs(cr - tr) >= error_allowed && fabs(cg - tg) >= error_allowed && fabs(cb - tb) >= error_allowed;
			
			if (found) {
				for (rx = -2; rx <= 2; rx++)
				for (ry = -2; ry <= 2; ry++)
					if (rx || ry) {
						r = pmap[y + ry][x + rx];
						if (r && sim->elements[TYP(r)].Properties & PROP_CONDUCTS && parts[ID(r)].life == 0 &&
							!(TYP(r) == PT_WATR || TYP(r) == PT_SLTW || TYP(r) == PT_NTCT || TYP(r) == PT_PTCT || TYP(r) == PT_INWR))
						{
							parts[ID(r)].life = 4;
							parts[ID(r)].ctype = TYP(r);
							sim->part_change_type(ID(r), x + rx, y + ry, PT_SPRK);
						}
					}
				return 0;
			}
		}

	return 0;
}
