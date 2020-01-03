#ifndef CYBERTRUCK_H
#define CYBERTRUCK_H

#include "simulation/ElementCommon.h"
#include "simulation/vehicles/vehicle.h"
#include <vector>

extern Vehicle CYBERTRUCK;

extern std::vector<VehiclePixel> CYBERTRUCK_PIXELS;
void draw_cybertruck(Renderer *ren, Particle *cpart, float rotation);

#endif