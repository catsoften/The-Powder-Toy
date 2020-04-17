#ifndef FORMAT_CIRCUIT_H
#define FOMRAT_CIRCUIT_H

#include "common/String.h"

const char inc_prefixes[] = {'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'};
const char dec_prefixes[] = {'m', 'u', 'n', 'p', 'f', 'a', 'z', 'y'};
const int prefix_count = 8;

String format_value(double value, const String &unit);

#endif