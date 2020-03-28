#ifndef CIRCUITS_RESISTANCE
#define CIRCUITS_RESISTANCE

#include "simulation/ElementCommon.h"
#include <unordered_map>

#define REALLY_BIG_RESISTANCE 10000000000000.0
#define SUPERCONDUCTING_RESISTANCE 1e-16

#define GROUND_TYPE PT_GRND

#define DIODE_V_THRESHOLD 0.7f
#define DIODE_V_BREAKDOWN 1000.0f

extern std::unordered_map<int, double> resistances;

bool valid_conductor(int typ, Simulation *sim, int i);
double get_resistance(int type, Particle *parts, int i, Simulation *sim);
double get_effective_resistance(int type, Particle *parts, int i, Simulation *sim);

#endif