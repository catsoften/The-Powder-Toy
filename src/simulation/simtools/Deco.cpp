#include "simulation/ToolCommon.h"

//#TPT-Directive ToolClass Tool_Deco TOOL_DECO 8
Tool_Deco::Tool_Deco() {
	Identifier = "DEFAULT_TOOL_DECO";
	Name = "DECO";
    Colour = PIXPACK(0xba85ff);
    Description = "Sets the dcolour of non-decorated particles to its element color.";
}

int Tool_Deco::Perform(Simulation * sim, Particle * cpart, int x, int y, int brushX, int brushY, float strength) {
	if (cpart && cpart->type)
        cpart->dcolour = 0xFF000000 + sim->elements[cpart->type].Colour;
	return 1;
}

Tool_Deco::~Tool_Deco() {}
