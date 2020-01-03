#include "simulation/ElementCommon.h"

/*
Bacteria features:
- Dies if it runs out of energy/food
- Dies if temperature is too high or low
- Dies if touches SOAP
- Infects STKM

Genes:
- Resistances:
   - Heat / Cold
   - Radiation
   - VIRS
   - SOAP / ACID
   - Other BCTR
- Metabolize different food sources
   - SUGR (default)
   - YEST (not that far of an evolutionary leap)
   - PLNT / photosynthesis
   - Other BCTR?
- Growth speed
   - How fast it consumes energy
   - Produces gas as it consumes (depends on food)
- Energy storage capacity
   - Spends energy to store however
- Spread vector
   - Stick to solids
   - Spread through air
   - Spread through liquids
   - dissolve into solution. WATR (ctype BCTR)
- Color
   - Fancy colors ooooh
   - Glow in the dark gene, costs energy but is cool

Bacteria can 'unlock' different sensory 'parts'
For example, range to sense food can be increased
Allows for finer control of movement but faster movement
costs more energy

Multiplier for speed of growth
Bacteria requires less sugar to grow as time goes on or if it has enough food
 */

//#TPT-Directive ElementClass Element_BCTR PT_BCTR 187
Element_BCTR::Element_BCTR()
{
	Identifier = "DEFAULT_PT_BCTR";
	Name = "BCTR";
	Colour = PIXPACK(0xDCF781);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
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

	Flammable = 1;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 90;
	HeatConduct = 150;
	Description = "Bacteria. Feed it sugar to grow, can evolve genes";

	Properties = TYPE_LIQUID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	DefaultProperties.life = 110;

	// Element properties here
	Update = &Element_BCTR::update;
	Graphics = &Element_BCTR::graphics;
}

//#TPT-Directive ElementHeader Element_BCTR static int update(UPDATE_FUNC_ARGS)
int Element_BCTR::update(UPDATE_FUNC_ARGS) {
	// update code here
	int r, rx, ry;
	for (rx = -1; rx < 2; rx++) { // Check area around bacteria for sugar
		for (ry = -1; ry < 2; ry++) {
			if (BOUNDS_CHECK) {
				r = pmap[y + ry][x + rx];
				if (TYP(r) != PT_SUGR) continue; // Sugar isn't here
				sim->kill_part(ID(r));
				// allow bacteria to expand here
			}
		}
	}
	return 0;
}

//#TPT-Directive ElementHeader Element_BCTR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_BCTR::graphics(GRAPHICS_FUNC_ARGS)
{
	// graphics code here
	// return 1 if nothing dymanic happens here
	
	return 0;
}

Element_BCTR::~Element_BCTR() {}
