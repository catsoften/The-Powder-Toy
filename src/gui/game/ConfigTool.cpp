#include "Tool.h"

#include "simulation/Simulation.h"
#include "simulation/ElementCommon.h"

#include "gui/Style.h"
#include "gui/interface/Window.h"
#include "gui/interface/Button.h"
#include "gui/interface/Label.h"
#include "gui/interface/Textbox.h"
#include "gui/interface/DropDown.h"
#include "gui/game/GameModel.h"

#include "graphics/Graphics.h"
#include "gui/interface/Engine.h"

#include "gui/game/config_tool/CRAY.h"
#include "gui/game/config_tool/FFGN.h"
#include "gui/game/config_tool/CLUD.h"
#include "gui/game/config_tool/TRBN.h"

#include "gui/game/config_tool/detector.h"
#include "gui/game/config_tool/channel.h"
#include "gui/game/config_tool/quantum.h"
#include "gui/game/config_tool/wavelength.h"
#include "gui/game/config_tool/powered.h"

/* List of config tool windows:
 * - DRAY
 * - QLOG
 * - Vehicles
 * - STAR
 * - PSTN
 * - EXFN
 * - NOTE
 * - PDTC
 *
 * - Powered: HSWC / STOR / PVOD / DLAY
 * - DTEC
 * - FFGN
 * - FILT / PHOT
 * - ION quantum state viewer
 * - WIFI / Portals
 * - CRAY
 * - CLUD
 */

// Determine which config element to start
void ConfigTool::Click(Simulation * sim, Brush * brush, ui::Point position) {
    int type = TYP(sim->pmap[position.Y][position.X]);
    if (!type) type = TYP(sim->photons[position.Y][position.X]);

    switch(type) {
        case PT_WIFI:
        case PT_PRTI:
        case PT_PRTO:
            new ConfigChannelWindow(this, sim, position);
            break;
        case PT_ION:
            new ConfigQuantumWindow(this, sim, position);
            break;
        case PT_FILT:
        case PT_PHOT:
            new ConfigWavelengthWindow(this, sim, position);
            break;
        case PT_FFGN:
            new ConfigFFGNWindow(this, sim, position);
            break;
        case PT_CRAY:
            new ConfigCRAYWindow(this, sim, position);
            break;
        case PT_CLUD:
            new ConfigCLUDWindow(this, sim, position);
            break;
        case PT_TRBN:
            new ConfigTRBNWindow(this, sim, position);
            break;
        case PT_DTEC:
        case PT_TSNS:
        case PT_PSNS:
        case PT_LSNS:
            new ConfigDetectorWindow(this, sim, position);
            break;
        case PT_DLAY:
        case PT_HSWC:
        case PT_PVOD:
        case PT_PUMP:
            new ConfigPoweredWindow(this, sim, position);
            break;
        default:
            new ConfigCRAYWindow(this, sim, position);
            break;
    }
}
