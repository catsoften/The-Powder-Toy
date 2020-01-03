#include "simulation/ElementCommon.h"

std::vector<std::pair<String, int> > quotes({
	std::make_pair(String("No stealing saves"), 0xFFFFFF),
	std::make_pair(String("CGI Detected!"), 0xFFFFFF),
	std::make_pair(String("No vote farming!"), 0xFFFFFF),
	std::make_pair(String("Your salvation is at hand!"), 0xFFFFFF),
	std::make_pair(String("No spamming!"), 0xFFFFFF),
	std::make_pair(String("BANHAMMER!"), 0xFF0000),
	std::make_pair(String("Thread locked!"), 0xFF0000),
	std::make_pair(String("Download my mod!"), 0xFFFFFF)
});

//#TPT-Directive ElementClass Element_JCB1 PT_JCB1 238
Element_JCB1::Element_JCB1()
{
	Identifier = "DEFAULT_PT_JCB1";
	Name = "JCB1";
	Colour = PIXPACK(0x094867);
	MenuVisible = 1;
	MenuSection = SC_CRACKER2;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = -.99f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = -1;

	HeatConduct = 61;
	Description = "Jacob1. Attacks FIGH.";

	Properties = TYPE_ENERGY | PROP_NOCTYPEDRAW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	DefaultProperties.life = 75;

	Create = &Element_CLST::create;
	Update = &Element_JCB1::update;
	Graphics = &Element_JCB1::graphics;
}

//#TPT-Directive ElementHeader Element_JCB1 static int update(UPDATE_FUNC_ARGS)
int Element_JCB1::update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties
	 * tmp     - Quote to display
	 * pavg[0] - Laser 1 angle
	 * pavg[1] - Laser 2 angle
	 */

	// Update quote
	parts[i].tmp = parts[i].tmp % quotes.size();
	if (sim->timer % 60 * 35 == 0)
		parts[i].tmp++;

	// Targetting
	int xdiff, ydiff, tx, ty;
	int count = 0;
	bool aggressive;
	for (unsigned int j = 0; j < MAX_FIGHTERS; ++j) {
		if (!sim->fighters[j].spwn)
			continue;
		xdiff = sim->fighters[j].legs[14] - parts[i].x;
		ydiff = sim->fighters[j].legs[15] - parts[i].y;
		aggressive = true;
		
		if (count == 0) { // Laser 1
			// Ease angle1 towards the FIGH
			parts[i].pavg[0] += (atan2(ydiff, xdiff) - parts[i].pavg[0]) / 20.0f;
			Element_SPDR::intersect_line(sim, x, y, cos(parts[i].pavg[0]), sin(parts[i].pavg[0]), tx, ty, 9);
			sim->CreateLine(x, y, tx, ty, PT_LASR);
		}
		else if (count == 0 || count == 1) { // Laser 2
			// Ease angle2 towards the FIGH
			parts[i].pavg[1] += (atan2(ydiff, xdiff) - parts[i].pavg[1]) / 20.0f;
			Element_SPDR::intersect_line(sim, x, y, cos(parts[i].pavg[1]), sin(parts[i].pavg[1]), tx, ty, 9);
			sim->CreateLine(x, y, tx, ty, PT_LASR);
		}
		if (RNG::Ref().chance(1, 50)) { // Guided missile
			int ni = sim->create_part(-1, x, y + (RNG::Ref().chance(1, 2) ? -2 : 2), PT_MSSL);
			parts[ni].pavg[0] = sim->fighters[j].legs[14];
			parts[ni].pavg[1] = sim->fighters[j].legs[15];
			parts[ni].life = 0;
		}

		++count;
	}

	if (!aggressive) {
		// Project force field
		int px = -1, py = -1, x1, y1;
		for (float angle = 0; angle < 2 * 3.1415f; angle += 0.01f) {
			x1 = x + 12 * cos(angle);
			y1 = y + 12 * sin(angle);
			if (x1 == px && y1 == py) // Avoid redrawing same spot
				continue;
			sim->create_part(-1, x1, y1, PT_FFLD);
			px = x1, py = y1;
		}
	}

	return 0;
}

//#TPT-Directive ElementHeader Element_JCB1 static int graphics(GRAPHICS_FUNC_ARGS)
int Element_JCB1::graphics(GRAPHICS_FUNC_ARGS) {
	ren->drawtext(cpart->x + 12, cpart->y - 7, quotes[cpart->tmp % quotes.size()].first,
		PIXR(quotes[cpart->tmp % quotes.size()].second),
		PIXG(quotes[cpart->tmp % quotes.size()].second),
		PIXB(quotes[cpart->tmp % quotes.size()].second), 255);

	*colr = *colg = *colb = *cola = 0;
	ren->addpixel(cpart->x + -2, cpart->y + -6, 50, 91, 111, 255);
	ren->addpixel(cpart->x + -1, cpart->y + -6, 23, 79, 112, 255);
	ren->addpixel(cpart->x + 0, cpart->y + -6, 3, 57, 95, 255);
	ren->addpixel(cpart->x + 1, cpart->y + -6, 8, 74, 109, 255);
	ren->addpixel(cpart->x + 2, cpart->y + -6, 54, 76, 89, 255);
	ren->addpixel(cpart->x + -2, cpart->y + -5, 7, 70, 114, 255);
	ren->addpixel(cpart->x + -1, cpart->y + -5, 9, 127, 214, 255);
	ren->addpixel(cpart->x + 0, cpart->y + -5, 4, 136, 218, 255);
	ren->addpixel(cpart->x + 1, cpart->y + -5, 1, 115, 186, 255);
	ren->addpixel(cpart->x + 2, cpart->y + -5, 21, 94, 126, 255);
	ren->addpixel(cpart->x + -2, cpart->y + -4, 14, 131, 211, 255);
	ren->addpixel(cpart->x + -1, cpart->y + -4, 2, 140, 228, 255);
	ren->addpixel(cpart->x + 0, cpart->y + -4, 5, 134, 218, 255);
	ren->addpixel(cpart->x + 1, cpart->y + -4, 18, 155, 235, 255);
	ren->addpixel(cpart->x + 2, cpart->y + -4, 1, 57, 104, 255);
	ren->addpixel(cpart->x + -2, cpart->y + -3, 11, 83, 133, 255);
	ren->addpixel(cpart->x + -1, cpart->y + -3, 3, 183, 244, 255);
	ren->addpixel(cpart->x + 0, cpart->y + -3, 4, 194, 246, 255);
	ren->addpixel(cpart->x + 1, cpart->y + -3, 10, 185, 252, 255);
	ren->addpixel(cpart->x + 2, cpart->y + -3, 14, 97, 139, 255);
	ren->addpixel(cpart->x + -2, cpart->y + -2, 9, 72, 103, 255);
	ren->addpixel(cpart->x + -1, cpart->y + -2, 8, 85, 131, 255);
	ren->addpixel(cpart->x + 0, cpart->y + -2, 5, 86, 142, 255);
	ren->addpixel(cpart->x + 1, cpart->y + -2, 13, 94, 147, 255);
	ren->addpixel(cpart->x + 2, cpart->y + -2, 18, 48, 72, 255);
	ren->addpixel(cpart->x + 0, cpart->y + -1, 3, 126, 187, 255);
	ren->addpixel(cpart->x + -1, cpart->y + 0, 3, 126, 187, 255);
	ren->addpixel(cpart->x + 1, cpart->y + 0, 3, 126, 187, 255);
	ren->addpixel(cpart->x + -1, cpart->y + 1, 3, 126, 187, 255);
	ren->addpixel(cpart->x + 1, cpart->y + 1, 3, 126, 187, 255);
	ren->addpixel(cpart->x + -1, cpart->y + 2, 3, 126, 187, 255);
	ren->addpixel(cpart->x + 2, cpart->y + 2, 3, 126, 187, 255);
	ren->addpixel(cpart->x + -1, cpart->y + 3, 3, 126, 187, 255);
	ren->addpixel(cpart->x + 2, cpart->y + 3, 3, 126, 187, 255);
	ren->addpixel(cpart->x + -2, cpart->y + 4, 3, 126, 187, 255);
	ren->addpixel(cpart->x + 2, cpart->y + 4, 3, 126, 187, 255);
	ren->addpixel(cpart->x + -2, cpart->y + 5, 3, 126, 187, 255);
	ren->addpixel(cpart->x + 2, cpart->y + 5, 3, 126, 187, 255);
	ren->addpixel(cpart->x + -2, cpart->y + 6, 3, 126, 187, 255);
	ren->addpixel(cpart->x + 1, cpart->y + 6, 3, 126, 187, 255);
	return 0;
}

Element_JCB1::~Element_JCB1() {}
