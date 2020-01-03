#include "simulation/ElementCommon.h"
#include "simulation/vehicles/vehicle.h"
#include "simulation/vehicles/kv2.h"

//#TPT-Directive ElementClass Element_TANK PT_TANK 213
Element_TANK::Element_TANK()
{
	Identifier = "DEFAULT_PT_TANK";
	Name = "TANK";
	Colour = PIXPACK(0x52482E);
	MenuVisible = 1;
	MenuSection = SC_CRACKER2;
	Enabled = 1;

	Advection = 0.01f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.15f; // NOTE: TANKS UPDATES TWICE PER FRAME
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 20;
	Description = "Soviet Tank. STKM can ride, comes with several upgrades. Hold down to shoot.";

	DefaultProperties.life = 600; // Default 600 HP

	Properties = TYPE_PART | PROP_NOCTYPEDRAW | PROP_VEHICLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_TANK::update;
	Graphics = &Element_TANK::graphics;
	ChangeType = &Element_TANK::changeType;
}

//#TPT-Directive ElementHeader Element_TANK static void changeType(ELEMENT_CHANGETYPE_FUNC_ARGS)
void Element_TANK::changeType(ELEMENT_CHANGETYPE_FUNC_ARGS) {
	if (to == PT_NONE && sim->parts[i].life <= 0) {
		int j, t;
		for (auto px = KV2_PIXELS.begin(); px != KV2_PIXELS.end(); ++px) {
			t = rand() % 100;
			if (t < 70)
				j = Element_CYTK::create_part(sim, px->x, px->y, PT_BRMT, sim->parts[i].pavg[0], sim->parts, i);
			else
				j = Element_CYTK::create_part(sim, px->x, px->y, PT_BREC, sim->parts[i].pavg[0], sim->parts, i);
			if (j > -1) {
				sim->parts[j].dcolour = 0xCF000000 | PIXRGB(px->r, px->g, px->b);
				sim->parts[j].vx = sim->parts[i].vx;
				sim->parts[j].vy = sim->parts[i].vy;
				sim->parts[j].temp = sim->parts[i].temp;
			}
		}
	}
}

//#TPT-Directive ElementHeader Element_TANK static int update(UPDATE_FUNC_ARGS)
int Element_TANK::update(UPDATE_FUNC_ARGS)
{
	// NOTE: TANK UPDATES TWICE PER FRAME

	/**
	 * Properties:
	 * vx, vy (velocity)
	 * ctype = element of STKM when it entered
	 * tmp2 = which STKM controls it (1 = STKM, 2 = STK2, 3 = AI car)
	 * tmp = rocket or flamethrower (0 none, 1 rocket, 2 flamethrower)
	 * life = HP
	 * pavg[0] = rotation
	 * pavg[1] = direction of travel (left or right)
	 * 
	 * If touched by a FIGH the FIGH will attempt to use the tank to run down
	 * the STKM and STK2
	 */

	// Collision checking and update
	// We'll consider moving in the direction of velocity
	// We only check collisions with 6 points on the rectangle that
	// bounds the tank
	float ovx = parts[i].vx, ovy = parts[i].vy;
	bool has_collision;
	Element_CYTK::initial_collision(sim, parts, i, KV2, has_collision);

	// Collision damage
	if (has_collision && (fabs(ovx) > KV2.COLLISION_SPEED || fabs(ovy) > KV2.COLLISION_SPEED)) {
		parts[i].life -= rand() % 25;
	}

	// Heat damage
	if (parts[i].temp > 273.15f + 200.0f)
		parts[i].life--;
	if (parts[i].temp > 3000.0f)
		parts[i].life = 0;

	// Self destruction, leave a randomized KV2 shaped pile of powder
	if (parts[i].life <= 0) {
		sim->kill_part(i);
		return 0;
	}

	// If life <= 150 spawn sparks (EMBR)
	if (parts[i].life <= 150) {
		if (rand() % 50 == 0)
			Element_CYTK::create_part(sim, KV2.WIDTH * 0.4f, -KV2.HEIGHT / 2, PT_EMBR, parts[i].pavg[0], parts, i);
		if (rand() % 50 == 0)
			Element_CYTK::create_part(sim, -KV2.WIDTH * 0.4f, -KV2.HEIGHT / 2, PT_EMBR, parts[i].pavg[0], parts, i);
		if (rand() % 50 == 0)
			Element_CYTK::create_part(sim, 0, -KV2.HEIGHT / 2, PT_EMBR, parts[i].pavg[0], parts, i);
	}
	// If life <= 300 spawn fire damage
	if (parts[i].life <= 300 && rand() % 30 == 0) {
		Element_CYTK::create_part(sim, -KV2.WIDTH * 0.4f, -KV2.HEIGHT / 2, PT_FIRE, parts[i].pavg[0], parts, i);
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
		Element_CYTK::get_target(sim, parts, tarx, tary);

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
			float vx = has_collision ? -KV2.SPEED : -KV2.FLY_SPEED / 8.0f, vy = 0.0f;
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
			Element_CYTK::exit_vehicle(sim, parts, i, x, y);
			return 0;
		}
		if (cmd2 == 4) { // Fly or shoot (down)
			if (parts[i].tmp == 1 || parts[i].tmp == 2) { // Flamethrower
				int j = Element_CYTK::create_part(sim, -KV2.WIDTH * 0.4f, -KV2.HEIGHT / 2, PT_BCOL, parts[i].pavg[0], parts, i);
				parts[j].life = rand() % 100 + 50;
				parts[j].vx = parts[i].pavg[1] ? 15 : -15;
				parts[j].vy = -(rand() % 3 + 3);
				if (parts[i].tmp == 1) // Plasma
					parts[j].temp = 9000.0f;
				rotate(parts[j].vx, parts[j].vy, parts[i].pavg[0]);
			}
			else if ((parts[i].tmp == 3 || parts[i].tmp == 0)) { // BOMB
				if (sim->timer % 50 == 0) {
					int j1 = Element_CYTK::create_part(sim, -KV2.WIDTH * 0.4f, -KV2.HEIGHT / 2,
						parts[i].tmp == 3 ? PT_BOMB : PT_FSEP, parts[i].pavg[0], parts, i);
					parts[j1].life = 20;
					parts[j1].vx = parts[i].pavg[1] ? 550 : -550;
					parts[j1].vy = 0;
					parts[j1].temp = 9999.0f;
					rotate(parts[j1].vx, parts[j1].vy, parts[i].pavg[0]);
				}

				// Turret flame when firing (Not with BOMB)
				if (parts[i].tmp != 3) {
					int j2 = Element_CYTK::create_part(sim, -KV2.WIDTH * 0.4f + 1, -KV2.HEIGHT / 2, PT_BANG, parts[i].pavg[0], parts, i);
					parts[j2].temp = 1500.0f;
				}
			}
		}
	}

	Element_CYTK::update_vehicle(sim, parts, i, KV2, ovx, ovy);
	return 0;
}

//#TPT-Directive ElementHeader Element_TANK static int graphics(GRAPHICS_FUNC_ARGS)
int Element_TANK::graphics(GRAPHICS_FUNC_ARGS)
{
	*cola = 0;
	draw_kv2(ren, cpart, cpart->pavg[0]);
	return 0;
}

Element_TANK::~Element_TANK() {}
