#include "simulation/ElementCommon.h"
#include "simulation/vehicles/horse.h"
#include "simulation/vehicles/vehicle.h"
#include <iostream>

//#TPT-Directive ElementClass Element_HRSE PT_HRSE 318
Element_HRSE::Element_HRSE() {
	Identifier = "DEFAULT_PT_HRSE";
	Name = "HRSE";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
	Enabled = 1;

	Advection = 0.01f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.1f;
	Diffusion = 0.0f;
	HotAir = 0.00f * CFDS;
	Falldown = 1;

	Flammable = 1;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;
	Weight = 50;

	DefaultProperties.temp = R_TEMP + 14.6f + 273.15f;
	HeatConduct = 60;
	Description = "Horse. Can be processed into GLUE, STKM can ride it, press down to dismount.";

	Properties = TYPE_PART | PROP_NOCTYPEDRAW | PROP_VEHICLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 620.0f;
	HighTemperatureTransition = PT_FIRE;

	DefaultProperties.life = 100;
	DefaultProperties.pavg[1] = -0.77f; // About 45 deg upwards default neck rotation

	Update = &Element_HRSE::update;
	Graphics = &Element_HRSE::graphics;
	Create = &Element_HRSE::create;
	CreateAllowed = &Element_HRSE::createAllowed;
	ChangeType = &Element_HRSE::changeType;
}

//#TPT-Directive ElementHeader Element_HRSE static void create(ELEMENT_CREATE_FUNC_ARGS)
void Element_HRSE::create(ELEMENT_CREATE_FUNC_ARGS) {
	if (RNG::Ref().chance(1, 2))
		sim->parts[i].tmp |= 2; // Randomize direction it's facing
}

//#TPT-Directive ElementHeader Element_HRSE static void changeType(ELEMENT_CHANGETYPE_FUNC_ARGS)
void Element_HRSE::changeType(ELEMENT_CHANGETYPE_FUNC_ARGS) {
	if (to == PT_NONE && sim->parts[i].life <= 0) {
		int rx, ry, r;
		for (rx = -2; rx <= 2; rx++)
		for (ry = -2; ry <= 2; ry++)
			if (BOUNDS_CHECK && (rx || ry)) {
				r = sim->pmap[y + ry][x + rx];
				if (!r) {
					sim->create_part(-1, x + rx, y + ry, PT_DUST);
					continue;
				}
				// Water -> GLUE
				if (sim->elements[TYP(r)].Properties & PROP_WATER && sim->parts[ID(r)].temp > 273.15f + 50.0f)
					sim->part_change_type(ID(r), x + rx, y + ry, PT_GLUE);
			}
	}
}

//#TPT-Directive ElementHeader Element_HRSE static int update(UPDATE_FUNC_ARGS)
int Element_HRSE::update(UPDATE_FUNC_ARGS) {
	// NOTE: HORSE UPDATES TWICE PER FRAME
	/* Properties:
	 * vx, vy (velocity)
	 * ctype = element of STKM when it entered
	 * tmp2 = which STKM controls it (0 = AI horse, 1 = STKM, 2 = STK2, 3 = FIGH)
	 * tmp | 1 = STKM rocket boot state or such
	 * tmp | 2 = Direction of travel (true = right, false = left)
	 * tmp | 4 = Fleeing to left
	 * tmp | 8 = Fleeing to right
	 * life = health
	 * pavg[0] = rotation
	 * pavg[1] = neck rotation
	 */
	float ovx = parts[i].vx, ovy = parts[i].vy;
	int rx, ry, r;
	bool has_collision;
	Element_CYTK::initial_collision(sim, parts, i, Horse, has_collision);

	// Self destruction
	if (parts[i].life <= 0) {
		sim->kill_part(i);
		return 0;
	}

	// Heat damage
	if (parts[i].temp > 273.15f + 50.0f)
		parts[i].life -= 2;
	// Pressure damage
	if (fabs(sim->pv[y / CELL][x / CELL]) > 1.0f)
		parts[i].life -= 2;

	// If life <= 25 drip BLOD
	if (parts[i].life <= 25) {
		sim->create_part(-1, x, y + 1, PT_BLOD);
		parts[i].life--;
	}

	int cmd = 0, cmd2 = 0;
	// Fleeing from danger
	if (parts[i].tmp & 4) {
		cmd = 1;
		if (RNG::Ref().chance(1, 150))
			parts[i].tmp &= ~4;
	}
	else if (parts[i].tmp & 8) {
		cmd = 2;
		if (RNG::Ref().chance(1, 150))
			parts[i].tmp &= ~8;
	}
	
	// Horse AI
	else if (parts[i].tmp2 == 0) {
		// Eat stuff
		int foodx = Horse.WIDTH / 2, foody = Horse.HEIGHT / 2;
		if (parts[i].tmp & 2) foodx = -foodx - 5; // 5 is correction offset for some reason
		rotate(foodx, foody, parts[i].pavg[0]);
		bool found_food = false;

		for (rx = -2; rx <= 2; rx++)
		for (ry = -2; ry <= 2; ry++)
			if (BOUNDS_CHECK && (x + foodx + rx >= 0 && x + foodx + rx < XRES && y + foody + ry >= 0 && y + foody + ry < YRES)) {
				r = pmap[y + foody + ry][x + foodx + rx];
				if (r && sim->elements[TYP(r)].Properties & PROP_EDIBLE) {
					parts[i].pavg[1] = 0;
					found_food = true;
					if (RNG::Ref().chance(1, 200)) {
						parts[i].life += sim->elements[TYP(r)].FoodValue;
						sim->kill_part(ID(r));
					}
				}
			}
		if (!found_food) {
			parts[i].pavg[1] = -0.77f; // Normal neck angle
			if (RNG::Ref().chance(1, 200))
				cmd = RNG::Ref().chance(1, 2) ? 1 : 2;
		}
	}

	// Player controls
	else if (parts[i].tmp2 == 1 || parts[i].tmp2 == 2) {
		int command = parts[i].tmp2 == 1 ? sim->player.comm : sim->player2.comm;
		if (((int)command & 0x01) == 0x01) // Left
			cmd = 1;
		if (((int)command & 0x02) == 0x02) // Right
			cmd = 2;
		if (((int)command & 0x04) == 0x04) // Up (Jump)
			cmd2 = 3;
		if (((int)command & 0x08) == 0x08) // Down (Exit)
			cmd2 = 4;
	}
	
	// Fighter AI
	else if (parts[i].tmp2 > 2) {
		// Get target and run towards it
		int tarx = -1, tary = -1;
		Element_CYTK::get_target(sim, parts, tarx, tary);
		if (tarx > 0 && has_collision)
			cmd = tarx > parts[i].x ? 2 : 1;
	}

	// Horse instinct that overrides any player controls
	for (rx = -1; rx <= 1; rx++)
	for (ry = -1; ry <= 1; ry++)
		if (BOUNDS_CHECK && (rx || ry)) {
			// Run away from hot air
			// Run away from pressure
			if (fabs(sim->pv[y / CELL + ry][x / CELL + rx]) > 0.2f ||
					fabs(sim->hv[y / CELL + ry][x / CELL + rx]) > 273.15f + 60.0f)
				goto end;
		}
	for (rx = -5; rx <= 5; rx++)
	for (ry = -5; ry <= 5; ry++)
		if ((x + rx < XRES && x + rx >= 0 && y + ry >= 0 && y + ry < YRES) && (rx || ry)) {
			r = pmap[y + ry][x + rx];
			if (!r) r = sim->photons[y + ry][x + rx];
			if (!r) continue;

			// Run away from hot things
			// Run away from DEADLY things
			if (parts[ID(r)].temp > 273.15f + 60.0f || sim->elements[TYP(r)].Properties & PROP_DEADLY)
				goto end;
		}
	if (false) {
		end:;
		parts[i].life -= 3;
		parts[i].tmp &= ~4;
		parts[i].tmp &= ~8;
		if (rx > 0) parts[i].tmp |= 4;
		if (rx < 0) parts[i].tmp |= 8;
		cmd = rx > 0 ? 1 : 2;
	}

	// Do controls
	if (cmd != 0 || cmd2 != 0) {
		if (has_collision || (parts[i].tmp & 1)) { // Accelerating only can be done on ground or if rocket
		 	float vx = has_collision ? -Horse.SPEED : -Horse.FLY_SPEED / 8.0f, vy = 0.0f;
			if (cmd == 1) { // Left
				rotate(vx, vy, parts[i].pavg[0]);
				parts[i].vx += vx;
				parts[i].vy += vy;
				parts[i].tmp |= 2; // Set face direction
				parts[i].y -= 0.5;
			}
			if (cmd == 2) { // Right
				vx *= -1;
				rotate(vx, vy, parts[i].pavg[0]);
				parts[i].vx += vx;
				parts[i].vy += vy;
				parts[i].tmp &= ~2; // Set face direction
				parts[i].y -= 0.5;
			}
		}
		if (cmd2 == 4) { // Exit (down)
			Element_CYTK::exit_vehicle(sim, parts, i, x, y);
			return 0;
		}
		if (cmd2 == 3) { // Jump
			float vx = 0.0f, vy = 0.0f;

			if (parts[i].tmp & 1) {
				vy = -Horse.FLY_SPEED / 4.0f;
				int j1 = Element_CYTK::create_part(sim, Horse.WIDTH * 0.4f, Horse.HEIGHT / 2, PT_PLSM, parts[i].pavg[0], parts, i);
				int j2 = Element_CYTK::create_part(sim, -Horse.WIDTH * 0.4f, Horse.HEIGHT / 2, PT_PLSM, parts[i].pavg[0], parts, i);
				if (j1 > -1 && j2 > -1) {
					parts[j1].temp = parts[j2].temp = 400.0f;
					parts[j1].life = RNG::Ref().between(0, 100) + 50;
					parts[j2].life = RNG::Ref().between(0, 100) + 50;
				}
			}
			else if (has_collision) {
				vy = -10;
			}

			rotate(vx, vy, parts[i].pavg[0]);
			parts[i].vx += vx;
			parts[i].vy += vy;
		}
	}

	Element_CYTK::update_vehicle(sim, parts, i, Horse, ovx, ovy);
	return 0;
}

//#TPT-Directive ElementHeader Element_HRSE static int graphics(GRAPHICS_FUNC_ARGS)
int Element_HRSE::graphics(GRAPHICS_FUNC_ARGS) {
	draw_horse(ren, cpart, cpart->vx, cpart->vy);
	return 0;
}

// This is defined if it's ever needed for some reason
//#TPT-Directive ElementHeader Element_HRSE static bool createAllowed(ELEMENT_CREATE_ALLOWED_FUNC_ARGS)
bool Element_HRSE::createAllowed(ELEMENT_CREATE_ALLOWED_FUNC_ARGS) { return true; }

Element_HRSE::~Element_HRSE() {}
