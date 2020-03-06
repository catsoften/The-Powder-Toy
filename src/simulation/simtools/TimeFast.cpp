#include "simulation/ElementCommon.h"
#include "simulation/ToolCommon.h"

//#TPT-Directive ToolClass Tool_FAST TOOL_FAST 13
Tool_FAST::Tool_FAST() {
    Identifier = "DEFAULT_TOOL_FAST";
    Name = "FAST";
    Colour = PIXPACK(0xff7438);
    Description = "Speed up local time.";
}

int Tool_FAST::Perform(Simulation *sim, Particle *cpart, int x, int y, int brushX, int brushY, float strength) {
    Element_CBMB::time_dilation(sim, x, y, 1, strength * sim->MAX_TIME_DILATION);
    return 1;
}

Tool_FAST::~Tool_FAST() {}
