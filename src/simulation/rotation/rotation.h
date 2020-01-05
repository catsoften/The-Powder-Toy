#ifndef ROTATION_HELPER_H
#define ROTATION_HELPER_H

#include <cmath>
#include <vector>
#include <unordered_map>
#include <map>

#include "gui/interface/Colour.h"
#include "gui/interface/Point.h"
#include "simulation/vehicles/vehicle.h" // rotate(x, y, angle)

typedef std::vector<std::pair<ui::Point, ui::Colour> > pixel_vector;
typedef std::vector<std::pair<ui::Point, int> > element_vector;

void rotate_around(int &x, int &y, int cx, int cy, float rotation);
void rotate_around(float &x, float &y, float cx, float cy, float rotation);

// 2 ints -> 1 unique int
inline int map_key(int x, int y) {
    return (x + y) * (x + y + 1) / 2 + y;
}

/**
 * Rotate a pixel vector by float radians, and write the result to
 * the specified pixel array (which will be cleared). This is a simple
 * approximation and should not be used for anything where accuracy is
 * needed
 */
template <typename T> void fastrot_vector(const T &base, float angle, T &out, float cx=0.0f, float cy=0.0f) {
    out.clear();
    for (auto &p : base) {
        int x = p.first.X, y = p.first.Y;
        rotate_around(x, y, cx, cy, angle);
        out.push_back(std::make_pair(ui::Point(x, y), p.second));
    }
}

#endif