#include "simulation/circuits/resistance.h"
#include <exception>

bool valid_conductor(int typ, Simulation *sim, int i) {
    if (typ == PT_SWCH)
        return sim->parts[i].life;
    /**
     * Exceptions:
     * - INST does not have PROP_CONDUCTS but should be considered a conductor
     * - VOLT needs conductive property to not break circuits
     * - VOID needs conductive property to allow connections to ground
     */
    return sim->elements[typ].Properties & PROP_CONDUCTS || typ == PT_INST || typ == PT_VOLT || typ == GROUND_TYPE;
}

float get_resistance(int type, Particle *parts, int i, Simulation *sim) {
    if (type <= 0 || type > PT_NUM) // Should never happen, the throw below is just in case you need to debug
        throw "Error: Invalid particle type found in get_resistance in circuit simulation";

    if (type == PT_VOLT) // Ideal voltage sources have 0 internal resistance
        return 0.0f;
    if (!valid_conductor(type, sim, i)) // Insulators, set to really high value
        return REALLY_BIG_RESISTANCE;

    switch (type) {
    case PT_CAPR:
        // Capacitor effective 'resistance' starts from 0 and goes to a really big number
        // as charge builds up. We're modeling a capacitor as a resistor because
        // it's easier. We set effective resistance to 0 when discharging to avoid all the voltage
        // disappearing into the capacitor itself
        if (parts[i].tmp2 == 0)
            return 0.0f;
        return parts[i].pavg[1];
    case PT_INDC:
        // Inductors have very high initial resistance when there is a positive change in current
        // that slowly reduces, and vice versa. Effective resistance is saved in pavg1
        if (parts[i].tmp2 == 0)
            return 0.0f;
        return parts[i].pavg[1];
    case PT_SWCH:
        if (parts[i].life)
            return 0.1f;       // On
        return REALLY_BIG_RESISTANCE; // Off
    case PT_RSTR:              // Stores resitivity in pavg0
        return parts[i].pavg[0];
    case PT_COPR:
        return 0.0168f;
    case PT_ZINC:
        return 0.059f;
    case PT_SICN:
        return 30000000.0f;
    case PT_METL:
    case PT_BMTL:
    case PT_BRMT:
        return 0.46f;
    case PT_CRBN:
        if (parts[i].temp < 100.0f) // Superconduction < 100 K
            return 0;
        return 5.0f;
    case PT_NEON:
    case PT_NBLE:
        return 0.4f;
    case PT_NICH:
        return 1.00f;
    case PT_TUNG:
        return 0.056f;
    case PT_MERC:
        if (parts[i].temp < 4.0f) // Superconduction < 4 K
            return 0;
        return 0.98f;
    case PT_LEAD:
        return 0.22f;
    case PT_WATR:
        return 20000000000.0f;
    case PT_SLTW:
    case PT_SWTR:
    case PT_IOSL:
        return 3000000000.0f;
    case PT_IRON:
        return 0.0971f;
    case PT_GOLD:
        return 0.0244;
    case PT_TTAN:
        return 0.43f;
    case PT_RBDM:
    case PT_LRBD:
        return 8.3f;
    case PT_CESM:
    case PT_LCSM:
        return 0.2f;
    case PT_PSCN:
    case PT_NSCN:
        return 32.0f;
    case PT_INST:
        return 0.0f;
    case PT_BRAS:
        return 0.60f;
    case PT_BSMH:
        return 1.29f;
    case PT_ALMN:
        return 0.0265f;
    }

    return 0.5f; // Default conductor resistance
}