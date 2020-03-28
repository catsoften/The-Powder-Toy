#include "simulation/circuits/format.h"
#include <cmath>

String format_value(double value, const String &unit) {
    if (fabs(value) == 0.0)
        return String::Build("0.0 ", unit);

    int degree = (int)(log10(value < 0 ? -value : value) / 3);
    double scaled = value * pow(1000, -degree);

    if (degree < 0) {
        if (-degree > prefix_count)
            return String::Build(value, " ", unit);
        return String::Build(scaled, " ", dec_prefixes[-degree - 1], unit);
    }
    else if (degree > 0) {
        if (degree > prefix_count)
            return String::Build(value, " ", unit);
        return String::Build(scaled, " ", inc_prefixes[degree - 1], unit);
    }
    return String::Build(value, " ", unit);
}