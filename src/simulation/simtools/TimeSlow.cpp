#include "simulation/ElementCommon.h"
#include "simulation/ToolCommon.h"

//#TPT-Directive ToolClass Tool_SLOW TOOL_SLOW 12
Tool_SLOW::Tool_SLOW() {
    Identifier = "DEFAULT_TOOL_SLOW";
    Name = "SLOW";
    Colour = PIXPACK(0x73e8fa);
    Description = "Slow down local time.";
}

int Tool_SLOW::Perform(Simulation *sim, Particle *cpart, int x, int y, int brushX, int brushY, float strength) {
    Element_CBMB::time_dilation(sim, x, y, 1, strength * sim->MIN_TIME_DILATION);
    return 1;
}

Tool_SLOW::~Tool_SLOW() {}
