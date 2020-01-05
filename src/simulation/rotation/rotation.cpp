#include "rotation.h"

/* Rotate around a point */
void rotate_around(int &x, int &y, int cx, int cy, float rotation) {
    x -= cx, y -= cy;
    rotate(x, y, rotation);
    x += cx, y += cy;
}

void rotate_around(float &x, float &y, float cx, float cy, float rotation) {
    x -= cx, y -= cy;
    rotate(x, y, rotation);
    x += cx, y += cy;
}