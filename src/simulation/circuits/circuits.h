#ifndef CIRCUITS_H
#define CIRCUITS_H

#include "simulation/CoordStack.h"
#include "simulation/ElementCommon.h"
#include <vector>

struct pos { short x, y; };
typedef std::vector<pos> coord_vec;

// coord_vec floodfill(Simulation *sim, Particle *parts, int x, int y);

#endif