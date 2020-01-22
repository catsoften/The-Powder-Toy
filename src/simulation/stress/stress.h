#ifndef STRESS_H
#define STRESS_H

#include "Config.h"

class Simulation;

class StressField {
public:
    Simulation * sim;
    bool enabled = false;
    float stress_map[YRES][XRES];
    float stress_map_x[YRES][XRES];
    float stress_map_y[YRES][XRES];

    StressField(Simulation * sim);
    void AggregateStress();
    void ComputeStress(int x, int y);
    void Clear();
private:
    
};

#endif