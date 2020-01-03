#include "simulation/ElementCommon.h"
#include "simulation/vehicles/vehicle.h"
#include "simulation/vehicles/cybertruck.h"

//#TPT-Directive ElementClass Element_CYTK PT_CYTK 190
Element_CYTK::Element_CYTK()
{
	Identifier = "DEFAULT_PT_CYTK";
	Name = "CYTK";
	Colour = PIXPACK(0x4D5564);
	MenuVisible = 1;
	MenuSection = SC_CRACKER2;
	Enabled = 1;

	Advection = 0.01f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.15f; // NOTE: CYBERTRUCK UPDATES TWICE PER FRAME
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 60;
	Description = "Tesla Cybertruck. STKM can ride, comes with several upgrades.";

	DefaultProperties.life = 100; // Default 100 HP

	Properties = TYPE_PART | PROP_NOCTYPEDRAW | PROP_VEHICLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_CYTK::update;
	Graphics = &Element_CYTK::graphics;
	ChangeType = &Element_CYTK::changeType;
}

//#TPT-Directive ElementHeader Element_CYTK static void changeType(ELEMENT_CHANGETYPE_FUNC_ARGS)
void Element_CYTK::changeType(ELEMENT_CHANGETYPE_FUNC_ARGS) {
	if (to == PT_NONE && sim->parts[i].life <= 0) {
		int j, t;
		for (auto px = CYBERTRUCK_PIXELS.begin(); px != CYBERTRUCK_PIXELS.end(); ++px) {
			t = rand() % 100;
			if (t < 20)
				j = create_part(sim, px->x, px->y, PT_BGLA, sim->parts[i].pavg[0], sim->parts, i);
			else if (t < 70)
				j = create_part(sim, px->x, px->y, PT_BRMT, sim->parts[i].pavg[0], sim->parts, i);
			else
				j = create_part(sim, px->x, px->y, PT_BREC, sim->parts[i].pavg[0], sim->parts, i);
			if (j > -1) {
				sim->parts[j].dcolour = 0xAF000000 | PIXRGB(px->r, px->g, px->b);
				sim->parts[j].vx = sim->parts[i].vx;
				sim->parts[j].vy = sim->parts[i].vy;
				sim->parts[j].temp = sim->parts[i].temp;
			}
		}
	}
}

//#TPT-Directive ElementHeader Element_CYTK static bool check_stkm(playerst *data, Simulation *sim, Particle *parts, int i)
bool Element_CYTK::check_stkm(playerst *data, Simulation *sim, Particle *parts, int i) {
	int xdiff = abs(data->legs[0] - parts[i].x);
	int ydiff = abs(data->legs[1] - parts[i].y);
	if (xdiff + ydiff < 10) {
		if (data->elem == PT_FIRE) // Flamethrower
			parts[i].tmp = 2;
		else if (data->rocketBoots) // Cyberthruster
			parts[i].tmp = 1;
		else if (data->elem == PT_BOMB) // Bomb
			parts[i].tmp = 3;
		parts[i].ctype = data->elem;
		sim->kill_part(data->stkmID);
		return true;
	}
	return false;
}

//#TPT-Directive ElementHeader Element_CYTK static bool attempt_move(int dx, int dy, Simulation *sim, Particle *parts, int i, const Vehicle &v)
bool Element_CYTK::attempt_move(int x, int y, Simulation *sim, Particle *parts, int i, const Vehicle &v) {
	if (parts[i].vx == 0 && parts[i].vy == 0)
		return false;

	rotate(x, y, parts[i].pavg[0]);
	x += parts[i].x;
	y += parts[i].y;

	int r, rx, ry;

	// Search for nearby vehicles
	for (auto ov = sim->vehicles.begin(); ov != sim->vehicles.end(); ++ov) {
		if (*ov == i) // Dont collide with self
			continue;
		float dx = fabs(parts[i].x - parts[*ov].x);
		float dy = fabs(parts[i].y - parts[*ov].y);

		if (dx < v.WIDTH && dy < v.HEIGHT) {
			if (fabs(parts[i].vx) > v.COLLISION_SPEED / 2.0f || fabs(parts[i].vy) > v.COLLISION_SPEED / 2.0f) {
				parts[*ov].life -= rand() % 40 + 10;
				parts[i].life -= rand() % 40 + 10;
			}
			parts[i].vx = 0;
			parts[i].vy = 0;

			// Tank destroys other vehicles
			if (parts[i].type == PT_TANK && parts[*ov].type != PT_TANK)
				parts[*ov].life = 0;
			break;
		}
	}

	// Test surrounding particles
	for (rx = -1; rx < 2; rx++)
		for (ry = -1; ry < 2; ry++)
			if (BOUNDS_CHECK) {
				if (sim->IsWallBlocking(x + rx, y + ry, PT_CYTK)) {
					parts[i].vx = parts[i].vy = 0;
					return true;
				}

				r = sim->pmap[y + ry][x + rx];
				if (!r) continue;

				// Cybertrucks heal from electrons
				if (parts[i].type == PT_CYTK && TYP(r) == PT_ELEC) {
					parts[i].life += 5;
					if (parts[i].life > 120)
						parts[i].life = 120;
				}

				if (TYP(r) != PT_CYTK && TYP(r) != PT_PRTI && TYP(r) != PT_PRTO &&
					(sim->elements[TYP(r)].Properties & TYPE_PART ||
					 sim->elements[TYP(r)].Properties & TYPE_SOLID)) {
					parts[i].vx = parts[i].vy = 0;
					return true;
				}
			}

	// Try moving in the direction of vx vy
	float largest = fabs(parts[i].vx) > fabs(parts[i].vy) ? fabs(parts[i].vx) : fabs(parts[i].vy);
	if (largest <= 1) { // Avoid "division by 0" effect where it ends up scanning the entire map
		return false;
	}
	float dvx = parts[i].vx / largest, dvy = parts[i].vy / largest;
	float sx = x, sy = y;
	int count = 1;

	while (sx >= 0 && sy >= 0 && sx < XRES && sy < YRES &&
			fabs(-dvx + dvx * count) <= fabs(parts[i].vx) && fabs(-dvy + dvy * count) <= fabs(parts[i].vy)) {
		r = sim->pmap[(int)round(sy)][(int)round(sx)];
		if (r) {
			if (TYP(r) != PT_CYTK && TYP(r) != PT_PRTI && TYP(r) != PT_PRTO &&
				(sim->elements[TYP(r)].Properties & TYPE_PART ||
				sim->elements[TYP(r)].Properties & TYPE_SOLID)) {
				parts[i].vx = parts[i].vy = 0;
				return true;
			}
		}

		sx += dvx; sy += dvy;
		count++;
	}
	return false;
}

//#TPT-Directive ElementHeader Element_CYTK static int create_part(Simulation *sim, int x, int y, int type, float theta, Particle *parts, int i)
int Element_CYTK::create_part(Simulation *sim, int x, int y, int type, float theta, Particle *parts, int i) {
	if (parts[i].pavg[1])
		x = -x; // Car going other way
	rotate(x, y, theta);
	x += parts[i].x; y += parts[i].y;
	return sim->create_part(-1, x, y, type);
}

//#TPT-Directive ElementHeader Element_CYTK static void initial_collision(Simulation *sim, Particle *parts, int i, const Vehicle &v, bool &has_collision)
void Element_CYTK::initial_collision(Simulation *sim, Particle *parts, int i, const Vehicle &v, bool &has_collision) {
	bool pbl = attempt_move(-v.WIDTH / 2, v.HEIGHT / 2, sim, parts, i, v);
	bool pbr = attempt_move(v.WIDTH / 2, v.HEIGHT / 2, sim, parts, i, v);
	bool pbc = attempt_move(0, v.HEIGHT / 2, sim, parts, i, v);
	bool tbl = attempt_move(-v.WIDTH / 2, -v.HEIGHT / 2, sim, parts, i, v);
	bool tbr = attempt_move(v.WIDTH / 2, -v.HEIGHT / 2, sim, parts, i, v);
	bool tbc = attempt_move(0, -v.HEIGHT / 2, sim, parts, i, v);
	has_collision = pbl || pbr || pbc || tbl || tbr || tbc;

	// Match terrain rotation
	if (pbl ^ pbr && !pbc) {
		parts[i].pavg[0] += pbl ? v.ROTATION_SPEED : -v.ROTATION_SPEED;
		parts[i].y -= 0.5;
	}
	if (tbl ^ tbr && !tbc) {
		parts[i].pavg[0] += tbl ? v.ROTATION_SPEED : -v.ROTATION_SPEED;
		parts[i].y += 0.5;
	}

	// If no collision rotate towards gravity every 10 frames
	if (!has_collision && sim->timer % 10 == 0)	{
		float target_angle = -200.0f; // Placeholder
		switch (sim->gravityMode) {
		default:
		case 0: // Normal, vertical gravity
			target_angle = 0;
			break;
		case 1: // No gravity
			break;
		case 2: // Radial gravity
			target_angle = atan2(parts[i].y - YRES / 2, parts[i].x - XRES / 2) + 3.1415 / 2;
			break;
		}

		if (target_angle == -200.0f) {}
		else if (parts[i].pavg[0] > target_angle)
			parts[i].pavg[0] -= v.ROTATION_SPEED;
		else
			parts[i].pavg[0] += v.ROTATION_SPEED;
	}

	// If sim stkm or stkm2 exists then stop allowing control to this car
	if (parts[i].tmp2 == 1 && sim->player.spwn)
		parts[i].tmp2 = 0;
	else if (parts[i].tmp2 == 2 && sim->player2.spwn)
		parts[i].tmp2 = 0;

	// Prevent spawning of stkm and stkm2 if inside car
	if (parts[i].tmp2 == 1)
		sim->vehicle_p1 = i;
	if (parts[i].tmp2 == 2)
		sim->vehicle_p2 = i;
}

///#TPT-Directive ElementHeader Element_CYTK static void get_target(Simulation *sim, Particle *parts, int &tarx, int &tary)
void Element_CYTK::get_target(Simulation *sim, Particle *parts, int &tarx, int &tary) {
	if (sim->player.spwn)
		tarx = sim->player.legs[0], tary = sim->player.legs[1];
	else if (sim->player2.spwn)
		tarx = sim->player2.legs[0], tary = sim->player2.legs[1];
	else if (sim->vehicle_p1 >= 0)
		tarx = parts[sim->vehicle_p1].x, tary = parts[sim->vehicle_p1].y;
	else if (sim->vehicle_p2 >= 0)
		tarx = parts[sim->vehicle_p2].x, tary = parts[sim->vehicle_p2].y;
}

///#TPT-Directive ElementHeader Element_CYTK static void exit_vehicle(Simulation *sim, Particle *parts, int i, int x, int y)
void Element_CYTK::exit_vehicle(Simulation *sim, Particle *parts, int i, int x, int y) {
	// Flag that vehicle has no stkm
	// Make sure new spawn location isn't too close or STKM
	// will instantly re-enter
	if (parts[i].tmp2 == 1) {
		sim->vehicle_p1 = -1;
		parts[i].tmp2 = 0;
		sim->player.stkmID = sim->create_part(-1, x - 15, y - 5, PT_STKM);
		sim->player.elem = parts[i].ctype;
		sim->player.rocketBoots = parts[i].tmp == 1;
	}
	else if (parts[i].tmp2 == 2) {
		sim->vehicle_p2 = -1;
		parts[i].tmp2 = 0;
		sim->player2.stkmID = sim->create_part(-1, x - 15, y - 5, PT_STKM2);
		sim->player2.elem = parts[i].ctype;
		sim->player2.rocketBoots = parts[i].tmp == 1;
	}
}

//#TPT-Directive ElementHeader Element_CYTK static void update_vehicle(Simulation *sim, Particle *parts, int i, const Vehicle &v, float ovx, float ovy)
void Element_CYTK::update_vehicle(Simulation *sim, Particle *parts, int i, const Vehicle &v, float ovx, float ovy) {
	// Limit max speed
	if (parts[i].vx > v.MAX_SPEED)
		parts[i].vx = v.MAX_SPEED;
	else if (parts[i].vx < -v.MAX_SPEED)
		parts[i].vx = -v.MAX_SPEED;
	if (parts[i].vy > v.MAX_SPEED)
		parts[i].vy = v.MAX_SPEED;
	else if (parts[i].vy < -v.MAX_SPEED)
		parts[i].vy = -v.MAX_SPEED;

	// Run over STKM and FIGH if fast enough
	if (fabs(ovx) > v.RUNOVER_SPEED || fabs(ovy) > v.RUNOVER_SPEED) {
		int xdiff = abs(sim->player.legs[0] - parts[i].x);
		int ydiff = abs(sim->player.legs[1] - parts[i].y);
		if (xdiff + ydiff < v.WIDTH / 2) {
			sim->kill_part(sim->player.stkmID);
		}

		xdiff = abs(sim->player2.legs[0] - parts[i].x);
		ydiff = abs(sim->player2.legs[1] - parts[i].y);
		if (xdiff + ydiff < v.WIDTH / 2) {
			sim->kill_part(sim->player2.stkmID);
		}

		for (unsigned int j = 0; j < MAX_FIGHTERS; ++j) {
			if (!sim->fighters[j].spwn)
				continue;
			xdiff = abs(sim->fighters[j].legs[0] - parts[i].x);
			ydiff = abs(sim->fighters[j].legs[1] - parts[i].y);
			if (xdiff + ydiff < v.WIDTH / 2)
				sim->kill_part(sim->fighters[j].stkmID);			
		}
	}

	// Check for nearby STKM and FIGH
	if (sim->vehicle_p1 < 0 && parts[i].tmp2 == 0 && sim->player.spawnID >= 0 && check_stkm(&sim->player, sim, parts, i)) {
		parts[i].tmp2 = 1;
		sim->vehicle_p1 = i;
	}
	if (sim->vehicle_p2 < 0 && parts[i].tmp2 == 0 && sim->player2.spawnID >= 0 && check_stkm(&sim->player2, sim, parts, i)) {
		parts[i].tmp2 = 2;
		sim->vehicle_p2 = i;
	}
	if (parts[i].tmp2 == 0) {
		for (unsigned int j = 0; j < MAX_FIGHTERS; ++j) {
			if (!sim->fighters[j].spwn)
				continue;
			if (check_stkm(&sim->fighters[j], sim, parts, i)) {
				parts[i].tmp2 = 3;
				break;
			}
		}
	}
}

//#TPT-Directive ElementHeader Element_CYTK static int update(UPDATE_FUNC_ARGS)
int Element_CYTK::update(UPDATE_FUNC_ARGS) {
	// NOTE: CYBERTRUCK UPDATES TWICE PER FRAME
	/**
	 * Properties:
	 * vx, vy (velocity)
	 * ctype = eleemnt of STKM when it entered
	 * tmp2 = which STKM controls it (1 = STKM, 2 = STK2, 3 = AI car)
	 * tmp = rocket or flamethrower (0 none, 1 rocket, 2 flamethrower)
	 * temp = damage
	 * life = charge
	 * pavg[0] = rotation
	 * pavg[1] = direction of travel (left or right)
	 * 
	 * If touched by a FIGH the FIGH will attempt to use the cybertruck to run down
	 * the STKM and STK2
	 */

	// Collision checking and update
	// We'll consider moving in the direction of velocity
	// We only check collisions with 6 points on the rectangle that
	// bounds the cybertruck
	float ovx = parts[i].vx, ovy = parts[i].vy;
	bool has_collision;
	initial_collision(sim, parts, i, CYBERTRUCK, has_collision);

	// Collision damage
	if (has_collision && (fabs(ovx) > CYBERTRUCK.COLLISION_SPEED || fabs(ovy) > CYBERTRUCK.COLLISION_SPEED)) {
		parts[i].life -= rand() % 25;
	}

	// Heat damage
	if (parts[i].temp > 273.15f + 200.0f)
		parts[i].life--;
	if (parts[i].temp > 1000.0f)
		parts[i].life = 0;

	// Self destruction, leave a randomized cybertruck shaped pile of powder
	if (parts[i].life <= 0) {
		sim->kill_part(i);
		return 0;
	}

	// If life <= 50 spawn sparks (EMBR)
	if (parts[i].life <= 50) {
		if (rand() % 50 == 0)
			create_part(sim, CYBERTRUCK.WIDTH * 0.4f, -CYBERTRUCK.HEIGHT / 2, PT_EMBR, parts[i].pavg[0], parts, i);
		if (rand() % 50 == 0)
			create_part(sim, -CYBERTRUCK.WIDTH * 0.4f, -CYBERTRUCK.HEIGHT / 2, PT_EMBR, parts[i].pavg[0], parts, i);
		if (rand() % 50 == 0)
			create_part(sim, 0, -CYBERTRUCK.HEIGHT / 2, PT_EMBR, parts[i].pavg[0], parts, i);
	}
	// If life <= 25 spawn fire damage
	if (parts[i].life <= 25 && rand() % 30 == 0) {
		create_part(sim, -CYBERTRUCK.WIDTH * 0.4f, -CYBERTRUCK.HEIGHT / 2, PT_FIRE, parts[i].pavg[0], parts, i);
	}

	// Player controls
	int cmd = 0, cmd2 = 0;
	if (parts[i].tmp2 == 1 || parts[i].tmp2 == 2) {
		int command = parts[i].tmp2 == 1 ? sim->player.comm : sim->player2.comm;
		if (((int)command & 0x01) == 0x01) // Left
			cmd = 1;
		if (((int)command & 0x02) == 0x02) // Right
			cmd = 2;
		if (((int)command & 0x04) == 0x04) // Up (Exit)
			cmd2 = 3;
		if (((int)command & 0x08) == 0x08) // Down (fly / shoot)
			cmd2 = 4;
	}
	
	// Fighter AI
	if (parts[i].tmp2 > 2) {
		// Get target
		int tarx = -1, tary = -1;
		get_target(sim, parts, tarx, tary);

		if (tarx > 0) {
			// Flamethrower / bomb weapon
			if (parts[i].tmp == 2 || parts[i].tmp == 3) {
				cmd2 = 4;
				parts[i].pavg[1] = tarx > parts[i].x;
			}
			// Rocket
			else if (parts[i].tmp == 1) {
				cmd = tarx > parts[i].x ? 2 : 1;
				cmd2 = tary < parts[i].y ? 4 : 0;
			}
			// Run 'em over
			else if (has_collision)
				cmd = tarx > parts[i].x ? 2 : 1;
		}
	}

	// Do controls
	if (cmd != 0 || cmd2 != 0) {
		if (has_collision || parts[i].tmp == 1) { // Accelerating only can be done on ground or if rocket
			float vx = has_collision ? -CYBERTRUCK.SPEED : -CYBERTRUCK.FLY_SPEED / 8.0f, vy = 0.0f;
			if (cmd == 1) { // Left
				rotate(vx, vy, parts[i].pavg[0]);
				parts[i].vx += vx;
				parts[i].vy += vy;
				parts[i].pavg[1] = 0; // Set face direction
				parts[i].y -= 0.5;
			}
			if (cmd == 2) { // Right
				vx *= -1;
				rotate(vx, vy, parts[i].pavg[0]);
				parts[i].vx += vx;
				parts[i].vy += vy;
				parts[i].pavg[1] = 1; // Set face direction
				parts[i].y -= 0.5;
			}
		}

		if (cmd2 == 3) { // Exit (up)
			exit_vehicle(sim, parts, i, x, y);
			return 0;
		}
		if (cmd2 == 4) { // Fly or shoot (down)
			if (parts[i].tmp == 1) { // Rocket
				float vx = 0.0f, vy = -CYBERTRUCK.FLY_SPEED / 4.0f;
				rotate(vx, vy, parts[i].pavg[0]);
				parts[i].vx += vx;
				parts[i].vy += vy;

				int j1 = create_part(sim, CYBERTRUCK.WIDTH * 0.4f, CYBERTRUCK.HEIGHT / 2, PT_PLSM, parts[i].pavg[0], parts, i);
				int j2 = create_part(sim, -CYBERTRUCK.WIDTH * 0.4f, CYBERTRUCK.HEIGHT / 2, PT_PLSM, parts[i].pavg[0], parts, i);
				parts[j1].temp = parts[j2].temp = 400.0f;
				parts[j1].life = rand() % 100 + 50;
				parts[j2].life = rand() % 100 + 50;
			}
			else if (parts[i].tmp == 2) { // Flamethrower
				int j = create_part(sim, -CYBERTRUCK.WIDTH * 0.4f, -CYBERTRUCK.HEIGHT / 2, PT_BCOL, parts[i].pavg[0], parts, i);
				parts[j].life = rand() % 100 + 50;
				parts[j].vx = parts[i].pavg[1] ? 15 : -15;
				parts[j].vy = -(rand() % 3 + 3);
				rotate(parts[j].vx, parts[j].vy, parts[i].pavg[0]);
			}
			else if (parts[i].tmp == 3 && sim->timer % 50 == 0) { // BOMB
				int j = create_part(sim, -CYBERTRUCK.WIDTH * 0.4f, -CYBERTRUCK.HEIGHT / 2, PT_BOMB, parts[i].pavg[0], parts, i);
				parts[j].life = rand() % 100 + 50;
				parts[j].vx = parts[i].pavg[1] ? 10 : -10;
				parts[j].vy = -(rand() % 3 + 3);
				rotate(parts[j].vx, parts[j].vy, parts[i].pavg[0]);
			}
		}
	}

	update_vehicle(sim, parts, i, CYBERTRUCK, ovx, ovy);
	return 0;
}

//#TPT-Directive ElementHeader Element_CYTK static int graphics(GRAPHICS_FUNC_ARGS)
int Element_CYTK::graphics(GRAPHICS_FUNC_ARGS) {
	*cola = 0;
	draw_cybertruck(ren, cpart, cpart->pavg[0]);
	return 0;
}

Element_CYTK::~Element_CYTK() {}
