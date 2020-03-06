#include "simulation/ElementCommon.h"
#include "simulation/ToolCommon.h"

//#TPT-Directive ToolClass Tool_RSET TOOL_RSET 11
Tool_RSET::Tool_RSET() {
    Identifier = "DEFAULT_TOOL_RSET";
    Name = "RSET";
    Colour = PIXPACK(0xfad673);
    Description = "Reset particle properties to default.";
}

int Tool_RSET::Perform(Simulation *sim, Particle *cpart, int x, int y, int brushX, int brushY, float strength) {
    if (!cpart)
        return 0;

    int rt = cpart->type;
    cpart->temp = sim->elements[rt].DefaultProperties.temp;
    cpart->tmp = sim->elements[rt].DefaultProperties.tmp;
    cpart->tmp2 = sim->elements[rt].DefaultProperties.tmp2;
    cpart->ctype = sim->elements[rt].DefaultProperties.ctype;
    cpart->life = sim->elements[rt].DefaultProperties.life;
    cpart->flags = sim->elements[rt].DefaultProperties.flags;
    cpart->vx = sim->elements[rt].DefaultProperties.vx;
    cpart->vy = sim->elements[rt].DefaultProperties.vy;
    if (sim->elements[rt].Create)
        sim->elements[rt].Create(sim, ID(sim->pmap[y][x]), x, y, rt, -1);
    return 1;
}

Tool_RSET::~Tool_RSET() {}
