#ifndef CIRCUITS_RESISTANCE
#define CIRCUITS_RESISTANCE

#include "simulation/ElementCommon.h"

bool valid_conductor(int typ, Simulation *sim, int i);
float get_resistance(int type, Particle *parts, int i, Simulation *sim);

#endif