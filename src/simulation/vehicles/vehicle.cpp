#include "simulation/ElementCommon.h"
#include "simulation/vehicles/vehicle.h"
#include "hmap.h"

#include <vector>
#include <cmath>

void rotate(int &x, int &y, float rotation) {
    if (fabs(rotation) < 0.01) return;
    int x2 = round(x * cos(rotation)) - round(y * sin(rotation));
    int y2 = round(y * cos(rotation)) + round(x * sin(rotation));
    x = x2; y = y2;
}

void rotate(float &x, float &y, float rotation) {
    if (fabs(rotation) < 0.01) return;
    float x2 = x * cos(rotation) - y * sin(rotation);
    float y2 = y * cos(rotation) + x * sin(rotation);
    x = x2; y = y2;
}

void draw_px_raw(const std::vector<VehiclePixel> &img, Renderer *ren, Particle *cpart, int cx, int cy, bool flip, float rotation) {
    int x, y;
    int caddress = restrict_flt((int)( restrict_flt((float)(cpart->temp+(-MIN_TEMP)), 0.0f, MAX_TEMP+(-MIN_TEMP)) / ((MAX_TEMP+(-MIN_TEMP))/1024) ) *3, 0.0f, (1024.0f*3)-3);
    for (auto px = img.begin(); px != img.end(); ++px) {
        x = px->x; y = px->y;

        // Flip horizontally if car is going other way
        if (flip) x = -x;
        rotate(x, y, rotation);

        // Heat display
        if (ren->colour_mode & COLOUR_HEAT) {
            ren->drawrect(cx + x, cy + y, 1, 1, color_data[caddress],  color_data[caddress + 1],  color_data[caddress + 2], 255);
        }
        // Normal display
        else {
            ren->drawrect(cx + x, cy + y, 1, 1, px->r, px->g, px->b, 255);

            // Blend if occupied
            if (cpart->tmp2 == 1) // Player 1
                ren->blendpixel(cx + x, cy + y, 255, 224, 160, 30);
            else if (cpart->tmp2 == 2) // Player 2
                ren->blendpixel(cx + x, cy + y, 100, 100, 255, 30);
            else if (cpart->tmp2 > 2) // FIGH
                ren->blendpixel(cx + x, cy + y, 255, 50, 0, 30);
        }
    }
}

void draw_px(const std::vector<VehiclePixel> &img, Renderer *ren, Particle *cpart, float rotation) {
    draw_px_raw(img, ren, cpart, cpart->x, cpart->y, cpart->pavg[1], rotation);
}