#ifndef GUNSHIP_H
#define GUNSHIP_H

#include "simulation/ElementCommon.h"
#include "simulation/vehicles/vehicle.h"
#include <vector>

extern Vehicle Gunship;

extern std::vector<VehiclePixel> GUNSHIP_BASE;
extern std::vector<VehiclePixel> THRUSTER1; // Front
extern std::vector<VehiclePixel> THRUSTER2; // Back
void draw_gunship(Renderer *ren, Particle *cpart, float vx, float vy);

#endif