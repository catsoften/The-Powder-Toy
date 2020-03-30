#include "simulation/circuits/resistance.h"
#include <exception>
#include <cmath>

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
    { PT_NEON, 10e3 }, // Neon
    { PT_NBLE, 539 },  // Helium
    { PT_NICH, 1e-7 },
    { PT_TUNG, 5.6e-8 },
    { PT_LEAD, 2.2e-8 },
    { PT_DSTW, 1820000000.0 },
    { PT_WATR, 1000 },
    { PT_SLTW, 30 },
    { PT_SWTR, 30 },
    { PT_IOSL, 30 },
    { PT_IRON, 9.7e-8 },
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
     * - VOID needs conductive property to allow connections to ground
     * 
     * Note: valid conductor only means if RSPK can exist on it, not if its currently capable of conducting
     * Ie, off SWCH should still return true because it has an ON state that can conduct
     */
    return sim->elements[typ].Properties & PROP_CONDUCTS || typ == PT_INST || typ == PT_VOLT || typ == GROUND_TYPE
        || typ == PT_SWCH || typ == PT_CAPR || typ == PT_INDC;
}

double get_resistance(int type, Particle *parts, int i, Simulation *sim) {
    if (type <= 0 || type > PT_NUM) // Should never happen, the throw below is just in case you need to debug
        throw "Error: Invalid particle type found in get_resistance in circuit simulation";
    if (!valid_conductor(type, sim, i)) // Insulators, set to really high value
        return REALLY_BIG_RESISTANCE;

    switch (type) {
        case PT_CAPR:
            // Capacitor effective 'resistance' starts from 0 and goes to a really big number
            // as charge builds up. We're modeling a capacitor as a resistor because
            // it's easier. We set effective resistance to 0 when discharging to avoid all the voltage
            // disappearing into the capacitor itself
            if (parts[i].tmp2 == 0)
                return 0.0;
            return parts[i].pavg[1];
        case PT_INDC:
            // Inductors have very high initial resistance when there is a positive change in current
            // that slowly reduces, and vice versa. Effective resistance is saved in pavg1
            if (parts[i].tmp2 == 0)
                return 0.0;
            return parts[i].pavg[1];
        case PT_RSTR:              // Stores resitivity in pavg0
            return parts[i].pavg[0];
        
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

    auto itr = resistances.find(type);
    if (itr != resistances.end())
        return itr->second;

    return 5e-7; // Random default conductor resistance
}

// Not base resistance, but actual resistance it behaves as in the circuit
// (ie, a switch can have either really low or really high resistance depending on state)
double get_effective_resistance(int type, Particle *parts, int i, Simulation *sim) {
    switch(type) {
        case PT_CAPR:
            return 0.0f;
        case PT_INDC:
            return 0.0f;
        case PT_SWCH:
            return parts[i].life >= 4 ? resistances[type] : REALLY_BIG_RESISTANCE;
    }
    return get_resistance(type, parts, i, sim);
}