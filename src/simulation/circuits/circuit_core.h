#ifndef CIRCUIT_CORE
#define CIRCUIT_CORE

#include <vector>

/* Defines for variable types to be less ambigious in code */
typedef int NodeId;
typedef int ElementType;

typedef double Ohms;
typedef double Volts;
typedef double Amps;

struct Pos { short x, y; };
typedef std::vector<Pos> coord_vec;

/* Config variables */
const int BASE_RSPK_LIFE = 4;
const int FORCE_RECALC_EVERY_N_FRAMES = 20;

// Divergence checking
const double WITHIN_STEADY_STATE = 0.0001;                // Error < value is basically steady state
const double INTEGRATION_TIMESTEP = 0.016666666666666666; // 1 frame = 1/60 s
const int INTEGRATION_RECALC_EVERY_N_FRAMES = 20;

// Component limits
const float MAX_CAPACITOR_CURRENT = 10000.0f;
const float MAX_CAPACITOR_REVERSE_CURRENT = 1.0f;

// Resistances
#define REALLY_BIG_RESISTANCE 10000000000000.0
#define SUPERCONDUCTING_RESISTANCE 1e-16

#define GROUND_TYPE PT_GRND

#define DIODE_V_THRESHOLD 0.7f
#define DIODE_V_BREAKDOWN 1000.0f

#endif