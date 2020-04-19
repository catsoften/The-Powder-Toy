#ifndef CIRCUITS_RESISTANCE
#define CIRCUITS_RESISTANCE

#include "simulation/ElementCommon.h"
#include "circuit_core.h"
#include <unordered_map>

extern std::unordered_map<ElementType, Ohms> resistances;

bool valid_conductor(int typ, Simulation *sim, int i);
double get_resistance(int type, Particle *parts, int i, Simulation *sim);
double get_effective_resistance(int type, Particle *parts, int i, Simulation *sim);

#endif