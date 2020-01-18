#include "simulation/ToolCommon.h"
#include "simulation/Air.h"

//#TPT-Directive ToolClass Tool_NORM TOOL_NORM 9
Tool_NORM::Tool_NORM() {
	Identifier = "DEFAULT_TOOL_NORM";
	Name = "NORM";
	Colour = PIXPACK(0x1aa336);
	Description = "Normalizes local temperature and pressure.";
}

int Tool_NORM::Perform(Simulation * sim, Particle * cpart, int x, int y, int brushX, int brushY, float strength) {
    sim->air->pv[y / CELL][x / CELL] -= sim->air->pv[y / CELL][x / CELL] * strength * 0.005f;
	if (!cpart)
		return 0;
	cpart->temp -= (cpart->temp - (273.15 + R_TEMP)) * 0.2f;
	return 1;
}

Tool_NORM::~Tool_NORM() {}
