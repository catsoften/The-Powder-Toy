#ifndef CIRCUITS_FRAMEWORK_H
#define CIRCUITS_FRAMEWORK_H

#include "simulation/CoordStack.h"
#include "simulation/ElementCommon.h"
#include <vector>

struct pos { short x, y; };
typedef std::vector<pos> coord_vec;

coord_vec floodfill(Simulation *sim, Particle *parts, int x, int y);
coord_vec coord_stack_to_skeleton(Simulation *sim, const coord_vec &floodfill);

#endif
