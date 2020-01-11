#include "simulation/ElementCommon.h"
#include "simulation/magnetics/magnetics.h"
#include <queue>
#include <iostream>

namespace RSPK {
const int REFRESH_EVERY_FRAMES = 20;

class VoltagePoint {
public:
	VoltagePoint(int x_, int y_, int counter_, float voltage_):
		x(x_), y(y_), counter(counter_), voltage(voltage_) {};
	int x, y, counter;
	float voltage;
};

float get_voltage_drop(int type) {
	if (type == PT_RSTR) {
		return 1.0f;
	}

	return 0; // Default no voltage drop
}

bool is_voltage_valid(Simulation *sim, Particle *parts, int x, int y, int counter, float voltage) {
	// New location must have an rspk
	int r = sim->photons[y][x];
	int r2 = sim->pmap[y][x];
	if (TYP(r) != PT_RSPK && !(sim->elements[TYP(r2)].Properties & PROP_CONDUCTS))
		return false;

	// New rspk location must not have been visited (tmp2 = 1)
	if (r && parts[ID(r)].tmp2 == 1)
		return false;

	// Die if no voltage
	if (voltage <= 0)
		return false;
	return true;
}

// Generates the voltage and charge movement gradient from a voltage source
void floodfill_voltage(Simulation *sim, Particle *parts, int x, int y, float voltage_i, int counter = 1) {
	std::queue<VoltagePoint*> queue;
	queue.push(new VoltagePoint(x, y, counter, voltage_i));
	float newvol;

	while (queue.size()) {
		VoltagePoint * p = queue.front();
		if (!sim->photons[p->y][p->x])
			sim->create_part(-3, p->x, p->y, PT_RSPK);

		if (parts[ID(sim->photons[p->y][p->x])].tmp2 > 0 && parts[ID(sim->photons[p->y][p->x])].tmp != 1)
			goto loopend;

		parts[ID(sim->photons[p->y][p->x])].pavg[0] = p->voltage;
		parts[ID(sim->photons[p->y][p->x])].tmp = p->counter;
		parts[ID(sim->photons[p->y][p->x])].tmp2 = 1;
		parts[ID(sim->photons[p->y][p->x])].life = REFRESH_EVERY_FRAMES + 1;
		
		// Reduce voltage
		newvol = p->voltage - get_voltage_drop(TYP(sim->pmap[p->y][p->x]));
		
		// Floodfill
		for (int rx = -1; rx <= 1; ++rx)
		for (int ry = -1; ry <= 1; ++ry)
			if ((rx || ry) && is_voltage_valid(sim, parts, p->x + rx, p->y + ry, p->counter + 1, newvol)) {
				queue.push(new VoltagePoint(p->x + rx, p->y + ry, p->counter + 1, newvol));
			}

		// Memory cleanup
		loopend:
		delete p;
		queue.pop();
	}
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
	 * tmp   - counter value, 1 = start, counts up as you go further
	 * tmp2  - value for floodfilling:
	 * 			0 = never visited
	 * 			1 = visited and valid connection
	 * pavg0 - Voltage
	 * 
	 * RSPK makes an electric field in the direction it goes
	 */
	int rx, ry, r;

	// Negate velocity, or it can be affected by Newtonian gravity
	parts[i].vx = parts[i].vy = 0;

	// Reset code
	if (sim->timer % RSPK::REFRESH_EVERY_FRAMES == 1 && parts[i].tmp != 1) // TODO remove tmp = 1, voltage sourcec keeps updating
		parts[i].tmp = parts[i].tmp2 = 0;

	// Kill on low life, life decrements below 0 if no longer connected to a voltage source
	// that regenerates life
	parts[i].life--;
	if (parts[i].life <= 0) {
		sim->kill_part(i);
		return 1;
	}
		

	// if (sim->timer % 20 == 1 && parts[i].tmp != 1) {
	// 	sim->kill_part(i);
	// 	return 1;
	// }

	// Temp code, move to create
	if (!parts[i].flags) {
		parts[i].flags = 1;
		parts[i].pavg[0] = 1000;
	}

	if (parts[i].tmp == 1) {
		RSPK::floodfill_voltage(sim, parts, x, y, parts[i].pavg[0]);
	}

	// Spread
	for (rx = -1; rx <= 1; ++rx)
	for (ry = -1; ry <= 1; ++ry)
		if (BOUNDS_CHECK && (rx || ry)) {
			r = sim->photons[y + ry][x + rx];
			if (r && TYP(r) == PT_RSPK) {
				// Found untouched region
				//if (parts[i].tmp2 == 1 && parts[ID(r)].tmp <= 0)
				//	RSPK::floodfill_voltage(sim, parts, x + rx, y + ry, parts[i].pavg[0], parts[i].tmp);
			}

			// r = pmap[y + ry][x + rx];
			// if (!r) continue;
			// if (sim->elements[TYP(r)].Properties & PROP_CONDUCTS && TYP(sim->photons[y + ry][x + rx]) != PT_RSPK)
			// 	sim->create_part(-3, x + rx, y + ry, PT_RSPK);
		}


	return 0;
}

//#TPT-Directive ElementHeader Element_RSPK static int graphics(GRAPHICS_FUNC_ARGS)
int Element_RSPK::graphics(GRAPHICS_FUNC_ARGS) {
	int x = 255 - (cpart->tmp % 255);
	*colr = *colg = *colb = x;

	if (cpart->tmp2 == 2) {
		*colr = 255;
		*colg = *colb = 0;
	}

	*firea = 255 - (cpart->tmp % 255); // 30;
	*firer = *colr / 2;
	*fireg = *colg / 2;
	*fireb = *colb / 2;
	*pixel_mode |= FIRE_SPARK;

	// if (cpart->tmp2 > 0)
	//	*colr = 0, *colg = 05, *colb = 0;
	return 0;
}

Element_RSPK::~Element_RSPK() {}
