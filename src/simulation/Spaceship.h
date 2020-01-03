#ifndef SPACESHIP_H
#define SPACESHIP_H
#define NUMSHIPCOMPONENTS 3

#include "simulation/ElementCommon.h"
#include <unordered_map>
#include <vector>
namespace SHIPS {
    extern std::unordered_map<int, std::vector<int>> ships;
    extern std::array<int, NUMSHIPCOMPONENTS> components;
    void createShip(int id);
    void translate(Simulation* sim, int id);
    void clonePAVG(Simulation* sim, int i, int x, int y);
    bool isShip(int id);
    bool isComponent(int typ);
}

#endif