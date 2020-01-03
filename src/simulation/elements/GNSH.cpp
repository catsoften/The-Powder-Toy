#include "simulation/ElementCommon.h"
#include "simulation/vehicles/vehicle.h"
#include "simulation/vehicles/gunship.h"
#include "gui/game/GameModel.h"

//#TPT-Directive ElementClass Element_GNSH PT_GNSH 214
Element_GNSH::Element_GNSH()
{
	Identifier = "DEFAULT_PT_GNSH";
	Name = "GNSH";
	Colour = PIXPACK(0x8fa7b3);
	MenuVisible = 1;
	MenuSection = SC_CRACKER2;
	Enabled = 1;

	Advection = 0.01f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.07f; // NOTE: GUNSHIP UPDATES TWICE PER FRAME
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 10;
	Description = "UEF T3 Heavy Gunship. Now rideable! UP = Shoot, DOWN = Fly, L + R + D = exit";

	DefaultProperties.life = 5000; // Default 5000 HP

	Properties = TYPE_PART | PROP_NOCTYPEDRAW | PROP_VEHICLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_GNSH::update;
	Graphics = &Element_GNSH::graphics;
}

//#TPT-Directive ElementHeader Element_GNSH static void accelerate_air(float rx, float ry, Simulation *sim, Particle *parts, int i, int x, int y, float tangle, float speed)
void Element_GNSH::accelerate_air(float rx, float ry, Simulation *sim, Particle *parts, int i, int x, int y, float tangle, float speed) {
	rotate(rx, ry, parts[i].pavg[0]);

	// Accelerate air
	sim->vx[(int)(y + ry) / CELL][(int)(x + rx) / CELL] += speed * cos(tangle);
	sim->vy[(int)(y + ry) / CELL][(int)(x + rx) / CELL] += speed * sin(tangle);

	// Search for nearby birds and kill 'em
	for (int rx2 = -3; rx2 <= 3; ++rx2)
		for (int ry2 = -3; ry2 <= 3; ++ry2)
			if (BOUNDS_CHECK && (rx2 || ry2)) {
				int r = sim->pmap[(int)(y + ry + ry2)][(int)(x + rx + rx2)];
				if (!r) continue;

				// Shred any birds :D
				if (TYP(r) == PT_BIRD)
					sim->part_change_type(ID(r), parts[ID(r)].x, parts[ID(r)].y, PT_BLOD);
			}
}

//#TPT-Directive ElementHeader Element_GNSH static int update(UPDATE_FUNC_ARGS)
int Element_GNSH::update(UPDATE_FUNC_ARGS) {
	// NOTE: TANK UPDATES TWICE PER FRAME

	/**
	 * Properties:
	 * vx, vy (velocity)
	 * ctype = element of STKM when it entered
	 * tmp2 = which STKM controls it (1 = STKM, 2 = STK2, 3 = AI car)
	 * tmp = plasma bolt (0) or laser (1) or missile (2)
	 * life = HP
	 * pavg[0] = rotation
	 * pavg[1] = direction of travel (left or right)
	 * 
	 * If touched by a FIGH the FIGH will attempt to use the gunship to fire on STKM
	 */

	// Collision checking and update
	// We'll consider moving in the direction of velocity
	// We only check collisions with 6 points on the rectangle that
	// bounds the tank
	float ovx = parts[i].vx, ovy = parts[i].vy;
	bool has_collision;
	Element_CYTK::initial_collision(sim, parts, i, Gunship, has_collision);

	// Heat damage
	if (parts[i].temp > 273.15f + 9000.0f)
		parts[i].life--;

	// Self destruction, leave a randomized Gunship shaped pile of powder
	if (parts[i].life <= 0) {
		sim->kill_part(i);
		return 0;
	}

	// If life <= 1000 spawn sparks (EMBR)
	if (parts[i].life <= 1000) {
		if (rand() % 50 == 0)
			Element_CYTK::create_part(sim, Gunship.WIDTH * 0.4f, -Gunship.HEIGHT / 2, PT_EMBR, parts[i].pavg[0], parts, i);
		if (rand() % 50 == 0)
			Element_CYTK::create_part(sim, -Gunship.WIDTH * 0.4f, -Gunship.HEIGHT / 2, PT_EMBR, parts[i].pavg[0], parts, i);
		if (rand() % 50 == 0)
			Element_CYTK::create_part(sim, 0, -Gunship.HEIGHT / 2, PT_EMBR, parts[i].pavg[0], parts, i);
	}
	// If life <= 300 spawn fire damage
	if (parts[i].life <= 300 && rand() % 30 == 0) {
		Element_CYTK::create_part(sim, -Gunship.WIDTH * 0.4f, -Gunship.HEIGHT / 2, PT_FIRE, parts[i].pavg[0], parts, i);
	}

	// Player controls
	int cmd = 0, cmd2 = 0;
	if (parts[i].tmp2 == 1 || parts[i].tmp2 == 2) {
		int command = parts[i].tmp2 == 1 ? sim->player.comm : sim->player2.comm;
		if (((int)command & 0x01) == 0x01) // Left
			cmd = 1;
		else if (((int)command & 0x02) == 0x02) // Right
			cmd = 2;
		if (((int)command & 0x03) == 0x03) // Left & Right
			cmd = 3;
		if (((int)command & 0x04) == 0x04) // Up (Shoot)
			cmd2 = 3;
		else if (((int)command & 0x08) == 0x08) // Down (fly)
			cmd2 = 4;
	}
	
	// Fighter AI
	// Get target
	int tarx = -1, tary = -1;
	if (parts[i].tmp2 > 2) {
		Element_CYTK::get_target(sim, parts, tarx, tary);
		if (tarx > 0) {
			cmd = tarx > parts[i].x ? 2 : 1;
			cmd2 = RNG::Ref().chance(1, 3) ? 3 : 0;
			if (RNG::Ref().chance(1, 10))
				cmd2 = tary < parts[i].y ? 3 : 4;
		}
	}

	// Do controls
	if (cmd != 0 || cmd2 != 0) {
		float vx = -Gunship.SPEED, vy = 0.0f;
		if (cmd == 1) { // Left
			rotate(vx, vy, parts[i].pavg[0]);
			parts[i].vx += vx;
			parts[i].vy += vy;
			parts[i].pavg[1] = 0; // Set face direction
			parts[i].y -= 0.5;
		}
		else if (cmd == 2) { // Right
			vx *= -1;
			rotate(vx, vy, parts[i].pavg[0]);
			parts[i].vx += vx;
			parts[i].vy += vy;
			parts[i].pavg[1] = 1; // Set face direction
			parts[i].y -= 0.5;
		}
		else if (cmd == 3 && cmd2 == 4) { // Exit (left and right and down)
			Element_CYTK::exit_vehicle(sim, parts, i, x, y);
			return 0;
		}
		// Thruster angle calculation
		float tangle = (fabs(vx) > 0.2f || fabs(vy) > 0.2f) ? atan2(vy, vx) : 3.1415f / 2;
		if (tangle > 3.1415f)
			tangle -= 3.1415f;

		if (cmd2 == 3) { // Shoot (up)
			// Hover while shooting
			float vx = 0.0f, vy = -0.08f;
			rotate(vx, vy, parts[i].pavg[0]);
			parts[i].vx += vx; parts[i].vy += vy;

			// Accelerate air
			accelerate_air(Gunship.WIDTH * 0.4f, Gunship.HEIGHT / 2, sim, parts, i, x, y, tangle, 7.0f);
			accelerate_air(-Gunship.WIDTH * 0.4f, Gunship.HEIGHT / 2, sim, parts, i, x, y, tangle, 7.0f);

			// Get target, mouse pos or fighter target
			std::pair<float, float> target = parts[i].tmp2 == 3 ?
				std::make_pair((float)tarx, (float)tary) : sim->getModel()->get_mouse_pos();
			float theta = atan2(target.second - parts[i].y, target.first - parts[i].x);

			if (parts[i].tmp == 1) { // Laser
				int tx, ty;
				Element_SPDR::intersect_line(sim, x, y, cos(theta), sin(theta), tx, ty, 9);
				sim->CreateLine(x + 8 * cos(theta), y + 3 * sin(theta), tx, ty, PT_LASR);
			}
			else if (parts[i].tmp != 1 && sim->timer % 50 == 0) { // Guided missile
				int ni = sim->create_part(-1, x + 8 * cos(theta), y + 3 * sin(theta), PT_MSSL);
				parts[ni].pavg[0] = target.first;
				parts[ni].pavg[1] = target.second;
				parts[ni].life = 0;
			}
		}
		else if (cmd2 == 4) { // Fly (down)
			float vx = 0.0f, vy = -Gunship.FLY_SPEED / 4.0f;
			rotate(vx, vy, parts[i].pavg[0]);
			parts[i].vx += vx;
			parts[i].vy += vy;

			// Accelerate air
			accelerate_air(Gunship.WIDTH * 0.4f, Gunship.HEIGHT / 2, sim, parts, i, x, y, tangle, 7.0f);
			accelerate_air(-Gunship.WIDTH * 0.4f, Gunship.HEIGHT / 2, sim, parts, i, x, y, tangle, 7.0f);

			int j1 = Element_CYTK::create_part(sim, Gunship.WIDTH * 0.4f, Gunship.HEIGHT / 2, PT_SMKE, parts[i].pavg[0], parts, i);
			int j2 = Element_CYTK::create_part(sim, -Gunship.WIDTH * 0.4f, Gunship.HEIGHT / 2, PT_SMKE, parts[i].pavg[0], parts, i);
			parts[j1].temp = parts[j2].temp = 400.0f;
			parts[j1].life = rand() % 100 + 50;
			parts[j2].life = rand() % 100 + 50;
		}
	}

	Element_CYTK::update_vehicle(sim, parts, i, Gunship, ovx, ovy);
	return 0;
}

//#TPT-Directive ElementHeader Element_GNSH static int graphics(GRAPHICS_FUNC_ARGS)
int Element_GNSH::graphics(GRAPHICS_FUNC_ARGS) {
	draw_gunship(ren, cpart, cpart->vx, cpart->vy);
	return 0;
}

Element_GNSH::~Element_GNSH() {}
