#ifndef VEHICLE_H
#define VEHICLE_H

#include "simulation/ElementCommon.h"
#include <vector>

/**
 * A type of vehicle
 * Note that most vehicles update TWICE per frame
 * 
 * - width: width in px
 * - height: height in px
 * - speed: vx when moving on ground
 * - flyspeed: velocity when flying
 * - maxspeed: vx and vy must be smaller in magnitude than this
 * - collision_speed: min speed for collision damage
 * - runover_speed: min speed to runover STKM
 * - rotation_speed: Radians to rotate per update 
 */
class Vehicle {
public:
    Vehicle(int w, int h, float speed, float flyspeed, float maxspeed, float collision_speed, float runover_speed, float rotation_speed):
        WIDTH(w), HEIGHT(h), SPEED(speed), FLY_SPEED(flyspeed), MAX_SPEED(maxspeed),
        COLLISION_SPEED(collision_speed), RUNOVER_SPEED(runover_speed), ROTATION_SPEED(rotation_speed) {}
    const int WIDTH, HEIGHT;
    const float SPEED, FLY_SPEED, MAX_SPEED, COLLISION_SPEED, RUNOVER_SPEED, ROTATION_SPEED;
};

class VehiclePixel {
public:
    VehiclePixel(int x_, int y_, int r_, int g_, int b_):
        x(x_), y(y_), r(r_), g(g_), b(b_) {}
    const int x, y, r, g, b;
};

void draw_px(const std::vector<VehiclePixel> &img, Renderer *ren, Particle *cpart, float rotation);
void draw_px_raw(const std::vector<VehiclePixel> &img, Renderer *ren, Particle *cpart, int cx, int cy, bool flip, float rotation);
void rotate(int &x, int &y, float rotation);
void rotate(float &x, float &y, float rotation);

#endif