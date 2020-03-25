#ifndef CIRCUITS_RESISTANCE
#define CIRCUITS_RESISTANCE

#include "simulation/ElementCommon.h"

#define REALLY_BIG_RESISTANCE 100000000000.0f
#define GROUND_TYPE PT_PAPR

#define DIODE_V_THRESHOLD 0.7f
#define DIODE_V_BREAKDOWN 1000.0f

bool valid_conductor(int typ, Simulation *sim, int i);
float get_resistance(int type, Particle *parts, int i, Simulation *sim);

#endif