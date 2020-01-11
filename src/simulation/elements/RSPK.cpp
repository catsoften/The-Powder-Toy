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

bool valid_conductor(int typ, Simulation *sim, int i) {
	if (typ == PT_SWCH)
		return sim->parts[i].life;
	return sim->elements[typ].Properties & PROP_CONDUCTS || typ == PT_INST;
}

float get_resistance(int type, Particle *parts, int i, Simulation *sim) {
	if (type <= 0 || type > PT_NUM) // Should never happen
		return 100.0f;
	if (!valid_conductor(type, sim, i)) // Most stuff isn't that doesn't conduct has uberhigh resistance
		return 100000000.0f;

	switch(type) {
		case PT_SWCH:
			if (parts[i].life)
				return 0.1f; // On
			return 10000000000.0f; // Off
		case PT_RSTR: // Stores resitivity in pavg0
			return parts[i].pavg[0];
		case PT_COPR:
			return 0.0168f;
		case PT_ZINC:
			return 0.059f;
		case PT_METL:
		case PT_BMTL:
		case PT_BRMT:
			return 0.46f;
		case PT_CRBN:
			if (parts[i].temp < 100.0f) // Superconduction < 100 K
				return 0;
			return 5.0f;
		case PT_NEON:
		case PT_NBLE:
			return 0.4f;
		case PT_NICH:
			return 1.00f;
		case PT_TUNG:
			return 0.056f;
		case PT_MERC:
			if (parts[i].temp < 4.0f) // Superconduction < 4 K
				return 0;
			return 0.98f;
		case PT_WATR:
			return 20000000000.0f;
		case PT_SLTW:
		case PT_SWTR:
		case PT_IOSL:
			return 3000000000.0f;
		case PT_IRON:
			return 0.0971f;
		case PT_GOLD:
			return 0.0244;
		case PT_TTAN:
			return 0.43f;
		case PT_RBDM:
		case PT_LRBD:
			return 8.3f;
		case PT_CESM:
		case PT_LCSM:
			return 0.2f;
		case PT_PSCN:
		case PT_NSCN:
			return 32.0f;
		case PT_INST:
			return 0.0f;
	}

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

bool is_ground(Particle *parts, int i) {
	if (!i) return false;
	if (parts[i].type == PT_VOID)
		return true;
	return false;
}

bool is_voltage_valid(Simulation *sim, Particle *parts, int x, int y, int counter, float voltage) {
	// New location must have an rspk
	int r = sim->photons[y][x];
	int r2 = sim->pmap[y][x];
	if (TYP(r) != PT_RSPK && !valid_conductor(TYP(r2), sim, ID(r2)))
		return false;

	// New rspk location must not have been visited (tmp2 = 1)
	if (r && parts[ID(r)].tmp2 == 1)
		return false;
	return true;
}

// Generates the voltage and charge movement gradient from a voltage source
void floodfill_voltage(Simulation *sim, Particle *parts, int x, int y, float voltage_i, int counter = 1) {
	std::queue<VoltagePoint*> queue;
	std::vector<int> ids;
	queue.push(new VoltagePoint(x, y, counter, voltage_i));
	float newvol, resistance, lowest_voltage = voltage_i;
	bool connected_to_ground = false, current_branch_past_ground = false;
	int id;

	while (queue.size()) {
		VoltagePoint * p = queue.front();
		if (!sim->photons[p->y][p->x])
			sim->create_part(-3, p->x, p->y, PT_RSPK);

		if (parts[ID(sim->photons[p->y][p->x])].tmp2 > 0 && parts[ID(sim->photons[p->y][p->x])].tmp != 1)
			goto loopend;

		id = ID(sim->photons[p->y][p->x]);
		resistance = get_resistance(TYP(sim->pmap[p->y][p->x]), parts, ID(sim->pmap[p->y][p->x]), sim);
		current_branch_past_ground = false;

		parts[id].pavg[0] = p->voltage;
		parts[id].pavg[1] = resistance;
		parts[id].tmp = p->counter;
		parts[id].tmp2 = 1;
		parts[id].life = REFRESH_EVERY_FRAMES + 1;
		ids.push_back(id);
		
		// Reduce voltage
		newvol = p->voltage - resistance;

		// Check if ground is connected
		for (int rx = -1; rx <= 1; ++rx)
		for (int ry = -1; ry <= 1; ++ry)
		if ((rx || ry)) {
			if (is_ground(parts, ID(sim->pmap[p->y + ry][p->x + rx]) )) {
				if (newvol < lowest_voltage)
					lowest_voltage = newvol;
				connected_to_ground = current_branch_past_ground = true;
				goto loopend;
			}
		}

		// Floodfill
		for (int rx = -1; rx <= 1; ++rx)
		for (int ry = -1; ry <= 1; ++ry)
		if ((rx || ry)) {
			if (!current_branch_past_ground && is_voltage_valid(sim, parts, p->x + rx, p->y + ry, p->counter + 1, newvol))
				queue.push(new VoltagePoint(p->x + rx, p->y + ry, p->counter + 1, newvol));
		}

		// Memory cleanup
		loopend:
		delete p;
		queue.pop();
	}

	// Make sure voltage decays to 0 at the end of the wire
	// Or no voltage difference exists without a ground
	if (ids.size() > 1) {
		float dv = voltage_i - lowest_voltage;
		for (unsigned int i = 0; i < ids.size(); i++) {
			if (connected_to_ground && parts[ids[i]].tmp != 1) {
				float percentin = (parts[ids[i]].pavg[0] - lowest_voltage) / dv;
				parts[ids[i]].pavg[0] = percentin * voltage_i;
				parts[ids[i]].pavg[1] *= voltage_i / dv;
			}
			else if (parts[ids[i]].tmp != 1) {
				// parts[ids[i]].pavg[0] = voltage_i;
				// parts[ids[i]].pavg[1] = 0.0f;
				sim->kill_part(ids[i]);
			}
		}
	}
}

};

//#TPT-Directive ElementClass Element_RSPK PT_RSPK 253
Element_RSPK::Element_RSPK()
{
	Identifier = "DEFAULT_PT_RSPK";
	Name = "VSPK";
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
	 * pavg1 - Voltage drop across pixel
	 * 
	 * RSPK makes an electric field in the direction it goes
	 */

	// Negate velocity, or it can be affected by Newtonian gravity
	parts[i].vx = parts[i].vy = 0;

	// Reset code
	if (sim->timer % RSPK::REFRESH_EVERY_FRAMES == 1 && parts[i].tmp != 1) // TODO remove tmp = 1, voltage sourcec keeps updating
		parts[i].tmp = parts[i].tmp2 = 0;

	// Kill on low life, life decrements below 0 if no longer connected to a voltage source
	// that regenerates life
	// Also kill self if no voltage drop (no current)
	// Also kill self if no longer on a conductor and not a source (tmp = 1)
	parts[i].life--;
	if (parts[i].life <= 0 ||
			// (parts[i].pavg[1] == 0.0f && parts[i].tmp != 1) ||
			(!RSPK::valid_conductor(TYP(pmap[y][x]), sim, ID(pmap[y][x])) && parts[i].tmp != 1)) {
		sim->kill_part(i);
		return 1;
	}

	if (sim->timer % RSPK::REFRESH_EVERY_FRAMES == 0 && parts[i].tmp == 1) {
		RSPK::floodfill_voltage(sim, parts, x, y, parts[i].pavg[0]);
	}

	// Heat up the conductor its on
	int r = pmap[y][x];
	float power = RSPK::get_power(x, y, sim);
	parts[ID(r)].temp += power / 500.0f;

	// Make sure self conductor can't be SPRKed
	parts[ID(r)].life = 4;

	// St Elmo's fire
	if (parts[i].pavg[0] > 10000000) {
		for (int rx = -1; rx <= 1; ++rx)
		for (int ry = -1; ry <= 1; ++ry)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = pmap[y + ry][x + rx];
				if (!r && RNG::Ref().chance(1, 100)) {
					int ni = sim->create_part(-1, x + rx ,y + ry, PT_PLSM);
					parts[ni].temp = parts[i].temp;
					parts[ni].life = RNG::Ref().between(0, 70);
				}
			}
	}

	return 0;
}

//#TPT-Directive ElementHeader Element_RSPK static int graphics(GRAPHICS_FUNC_ARGS)
int Element_RSPK::graphics(GRAPHICS_FUNC_ARGS) {
	// Power = V^2 / R and is used for brightness
	float power = RSPK::get_power(nx, ny, ren->sim);
	*firea = clamp_flt(power / 10.0f, 0, 500);

	*firer = *colr / 2;
	*fireg = *colg / 2;
	*fireb = *colb / 2;
	*pixel_mode |= FIRE_SPARK;
	*cola = *firea;
	return 0;
}

Element_RSPK::~Element_RSPK() {}
