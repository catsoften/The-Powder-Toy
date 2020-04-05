#ifndef CIRCUITS_FRAMEWORK_H
#define CIRCUITS_FRAMEWORK_H

#include "simulation/CoordStack.h"
#include "simulation/ElementCommon.h"
#include <vector>

class Circuits;

struct pos { short x, y; };
typedef std::vector<pos> coord_vec;

bool allow_conduction(int totype, int fromtype);
bool positive_terminal(int type);
bool negative_terminal(int type);
bool is_terminal(int type);
bool can_be_node(int i, Simulation * sim);

coord_vec floodfill(Simulation *sim, Particle *parts, int x, int y);
coord_vec coord_stack_to_skeleton(Simulation *sim, const coord_vec &floodfill);

#endif

