#include "simulation/circuits/circuits.h"
#include "simulation/ElementCommon.h"

/**
 * Construct and compute values for a circuit. This should only be called if
 * a circuit was physically modified (particles added or removed), otherwise
 * the circuit state should be directly calculated from the mesh
 */
// coord_vec floodfill(Simulation *sim, Particle *parts, int x, int y) {

// }





// float get_power(int x, int y, Simulation *sim)
// {
//     int r = sim->photons[y][x];
//     float voltage = sim->parts[ID(r)].pavg[1];
//     r = sim->pmap[y][x];
//     float resistance = get_resistance(TYP(r), sim->parts, ID(r), sim);
//     if (resistance == 0.0f)
//         return 0.0f;
//     return voltage / resistance * voltage;
// }

// /**
//  * Returns type of ground
//  * 0 = Not a ground
//  * 1 = Ground voltage is 0
//  * 2 = Don't multiply voltage drop
//  */
// int is_ground(Particle *parts, int i)
// {
//     if (!i)
//         return 0;
//     if (parts[i].type == PT_VOID)
//         return 1;
//     else if (parts[i].type == PT_VCMB)
//         return 2;
//     return 0;
// }

// bool is_voltage_valid(Simulation *sim, Particle *parts, int x, int y, int counter, float voltage)
// {
//     // New location must have an rspk
//     int r = sim->photons[y][x];
//     int r2 = sim->pmap[y][x];
//     if (TYP(r) != PT_RSPK && !valid_conductor(TYP(r2), sim, ID(r2)))
//         return false;

//     // New rspk location must not have been visited (tmp2 = 1)
//     if (r && parts[ID(r)].tmp2 == 1)
//         return false;
//     return true;
// }