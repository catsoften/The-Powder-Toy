#include "simulation/circuits/resistance.h"
#include "simulation/circuits/framework.h"
#include <exception>
#include <cmath>
#include <iostream>

// All resistances
std::unordered_map<int, double> resistances({
    { PT_SWCH, 1e-8 },
    // PT_VOLT's resistance only matters when a voltage source is not connected correctly,
    // so we set it really high to block current through incorrectly placed voltage sources
    // (Default is 0 if correctly connected)
    { PT_VOLT, REALLY_BIG_RESISTANCE },
    { GROUND_TYPE, SUPERCONDUCTING_RESISTANCE },
    { PT_COPR, 1.72e-8 },
    { PT_ZINC, 5.5e-8 },
    { PT_SICN, 30 },
    { PT_METL, 1e-7 }, // Metal is "steel"
    { PT_BMTL, 1e-7 },
    { PT_BRMT, 1e-7 },
    { PT_TRUS, 1e-7 },
    { PT_INWR, 1e-7 },
    { PT_BREC, 1e-7 },
    { PT_NEON, 10e3 }, // Neon
    { PT_NBLE, 539 },  // Helium
    { PT_HELM, 539 },
    { PT_PLSM, 539 },
    { PT_NICH, 1e-7 },
    { PT_TUNG, 5.6e-8 },
    { PT_LEAD, 2.2e-8 },
    { PT_DSTW, 1820000000.0 },
    { PT_WATR, 1000 },
    { PT_SLTW, 30 },
    { PT_SWTR, 30 },
    { PT_IOSL, 30 },
    { PT_IRON, 9.7e-8 },
    { PT_THRM, 9.7e-8 },
    { PT_GOLD, 2.44e-8 },
    { PT_TTAN, 4.2e-7 },
    { PT_RBDM, 1.2e-7 },
    { PT_LRBD, 1.2e-7 },
    { PT_CESM, 2e-7 },
    { PT_LCSM, 2e-7 },
    { PT_PSCN, 30 },
    { PT_NSCN, 30 },
    { PT_INST, SUPERCONDUCTING_RESISTANCE },
    { PT_BRAS, 0.6e-7 },
    { PT_BSMH, 1.3e-6 },
    { PT_ALMN, 2.65e-8 },
    { PT_PLUT, 141.4e-8 },
    { PT_URAN, 2.8e-7 },
    { PT_POLO, 40e-8 },
    { PT_THOR, 1.5e-7 },
    { PT_PHSP, 1e-7 }
});

// Functions
bool valid_conductor(int typ, Simulation *sim, int i) {
    if (i < 0 || !typ) return false;

    /**
     * Exceptions:
     * - INST does not have PROP_CONDUCTS but should be considered a conductor
     * - VOLT needs conductive property to not break circuits
     * - GRND needs conductive property to allow connections to ground
     * - CAPR and INDC - same logic as VOLT
     * - QRTZ / PQRT - sometimes conductive
     * - PLSM, HELM - doesn't conduct SPRK but should conduct RSPK
     * 
     * Note: valid conductor only means if RSPK can exist on it, not if its currently capable of conducting
     * Ie, off SWCH should still return true because it has an ON state that can conduct
     */
    return sim->elements[typ].Properties & PROP_CONDUCTS || typ == PT_INST || typ == PT_VOLT || typ == GROUND_TYPE
        || typ == PT_SWCH || typ == PT_CAPR || typ == PT_INDC || typ == PT_QRTZ || typ == PT_PQRT
        || typ == PT_PLSM || typ == PT_HELM;
}

/**
 * Use get_resistance or get_effective_resistance?
 * If your element updates its resistance every frame like a thermoresistor (or could, like a switch)
 * then use get_effective_resistance, otherwise use get_resistance, which updates only every couple of frames
 * 
 * If you set a resistance in effective_resistance, set the resistance in get_resistance to 0.0
 * Also dynamic resistance elements should be added to framework.cpp:dynamic_resistor
 */

double get_resistance(int type, Particle *parts, int i, Simulation *sim) {
    if (type <= 0 || type > PT_NUM) // Should never happen, the throw below is just in case you need to debug
        throw "Error: Invalid particle type found in get_resistance in circuit simulation";
    if (!valid_conductor(type, sim, i)) // Insulators, set to really high value
        return REALLY_BIG_RESISTANCE;

    switch (type) {
        case PT_CAPR:
            // Capacitor resistance is only considered
            // when numeric integration diverges, in which
            // we replace it with a open circuit (or really large resistance)
            return REALLY_BIG_RESISTANCE;
        case PT_INDC:
            // Inductor resistance is only considered
            // when numeric integration diverges, in which
            // we replace it with a short
            return 0.0;
        case PT_RSTR: // Stores resitivity in pavg0
            return parts[i].pavg[0];

        // These are dynamic, set to 0
        case PT_QRTZ:
        case PT_PQRT:
        case PT_MERC:
        case PT_CRBN:
        case PT_PTCT:
        case PT_NTCT:
            return 0.0;
    }

    if (negative_resistance(type)) return 0.0;

    auto itr = resistances.find(type);
    if (itr != resistances.end())
        return itr->second;

    return 5e-7; // Random default conductor resistance
}

// Not base resistance, but actual resistance it behaves as in the circuit
// (ie, a switch can have either really low or really high resistance depending on state)
// This updates every frame, if your element has a dynamic resistance set the resistance
// in get resistance to 0.0.
double get_effective_resistance(int type, Particle *parts, int i, Simulation *sim) {
    switch(type) {
        case PT_SWCH:
            return parts[i].life >= 4 ? resistances[type] : REALLY_BIG_RESISTANCE;

        // Quartz
        case PT_QRTZ:
        case PT_PQRT:
            return parts[i].temp < 173.15f ? 50 : REALLY_BIG_RESISTANCE;

        // Superconductors
        case PT_MERC:
            return parts[i].temp < 4 ? SUPERCONDUCTING_RESISTANCE : 9.6e-7;
        case PT_CRBN: // Unrealistic critical temp, but matches behavior for SPRK
            return parts[i].temp < 100 ? SUPERCONDUCTING_RESISTANCE : 1e-5;
            
        // Semiconductors
        case PT_PTCT: // Resistance goes to 1e-7 above 100 C
            if (parts[i].temp <= 373.15f)
                return REALLY_BIG_RESISTANCE;
            return std::max(-(double)log((parts[i].temp - 373.15f) / 10.0f), 1e-7);
        case PT_NTCT: // Resistance goes to 1e-7 below 100 C
            if (parts[i].temp >= 373.15f)
                return REALLY_BIG_RESISTANCE;
            return std::max(-(double)log(-(parts[i].temp - 373.15f) / 10.0f), 1e-7);
    }

    // Negative resistance conductors
    if (negative_resistance(type)) {
        double base_resistance = resistances[type];
        double current = 0.0;
        int r = sim->photons[(int)(parts[i].y + 0.5f)][(int)(parts[i].x + 0.5f)];
        if (r && TYP(r) == PT_RSPK)
            current = 1000 * fabs(parts[ID(r)].pavg[1]);
        return base_resistance / (1 + current);
    }

    return get_resistance(type, parts, i, sim);
}