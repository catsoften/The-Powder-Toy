#include "gui/game/Tool.h"

#include "simulation/Simulation.h"
#include "simulation/ElementCommon.h"

#include "gui/Style.h"
#include "gui/interface/Window.h"
#include "gui/interface/Button.h"
#include "gui/interface/Label.h"
#include "gui/interface/Textbox.h"
#include "gui/interface/Dropdown.h"
#include "gui/game/GameModel.h"

#include "graphics/Graphics.h"
#include "gui/interface/Engine.h"
#include "gui/game/config_tool/util.h"
#include "Misc.h"

class ConfigTRBNWindow: public ui::Window {
public:
	ConfigTool * tool;
	Simulation * sim;
	ui::Point pos;
	ui::Textbox * pavg0, * pavg1;
	ui::Label * messageLabel;

	int pid;
	float pavg0_val, pavg1_val;

	ConfigTRBNWindow(ConfigTool *tool_, Simulation *sim_, ui::Point partpos);
    void DoDraw() override {
        ui::Window::DoDraw();
    }
    void DoMouseMove(int x, int y, int dx, int dy) override {
        ui::Window::DoMouseMove(x, y, dx, dy);
    }
    void DoMouseDown(int x, int y, unsigned button) override {
        ui::Window::DoMouseDown(x, y, button);
    }
    void OnDraw() override {
        Graphics * g = GetGraphics();
        g->clearrect(Position.X-2, Position.Y-2, Size.X+3, Size.Y+3);
        g->drawrect(Position.X, Position.Y, Size.X, Size.Y, 200, 200, 200, 255);
    }
	void DoMouseUp(int x, int y, unsigned button) override {
        ui::Window::DoMouseUp(x, y, button);
	}
	void DoMouseWheel(int x, int y, int d) override {
		ui::Window::DoMouseWheel(x, y, d);
	}
	void DoKeyPress(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) override {
		ui::Window::DoKeyPress(key, scan, repeat, shift, ctrl, alt);
	}
	void DoKeyRelease(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) override {
		ui::Window::DoKeyRelease(key, scan, repeat, shift, ctrl, alt);
	}
	virtual ~ConfigTRBNWindow() {}
    void OnTryExit(ui::Window::ExitMethod method) override {
        if (method == ui::Window::MouseOutside)
           NOT_NEAR_WINDOW_BOTTOM_CHECK()
        CloseActiveWindow();
        SelfDestruct();
    }
};

ConfigTRBNWindow::ConfigTRBNWindow(ConfigTool * tool_, Simulation * sim_, ui::Point partpos):
	ui::Window(partpos, ui::Point(190, 82)),
	tool(tool_),
	sim(sim_),
    pos(partpos)
{
	// Move window near the cloud
	align_window_near(Position, partpos, Size);

	int pr = sim->pmap[pos.Y][pos.X];
	pid = ID(pr);

	messageLabel = make_left_label(ui::Point(4, 5), ui::Point(Size.X - 8, 15), "Config TRBN");
	messageLabel->SetTextColour(style::Colour::InformationTitle);
	AddComponent(messageLabel);

	// Pavg0
	ui::Label * tempLabel = make_left_label(ui::Point(4, 24), ui::Point(100, 15), "Min air vel.");
	AddComponent(tempLabel);

    pavg0_val = sim->parts[pid].pavg[0];
	pavg0 = make_left_textbox(ui::Point(100, 24), ui::Point(80, 15), String::Build(pavg0_val), "0.3");
    FLOAT_INPUT(pavg0, pavg0_val)
    AddComponent(pavg0);

	// Pavg1
	tempLabel = make_left_label(ui::Point(4, 44), ui::Point(100, 15), "Min part. vel.");
	AddComponent(tempLabel);

	pavg1_val = sim->parts[pid].pavg[1];
	pavg1 = make_left_textbox(ui::Point(100, 44), ui::Point(80, 15), String::Build(pavg1_val), "1.0");
	FLOAT_INPUT(pavg1, pavg1_val)
    AddComponent(pavg1);

	// Rest of window
	ui::Button * okayButton = make_center_button(ui::Point(Size.X / 2, Size.Y-16), ui::Point(Size.X / 2, 16), "OK");
	okayButton->SetActionCallback({ [this] {
		// Set property
        PropertyValue value;
        value.Float = pavg0_val;
        sim->flood_prop(pos.X, pos.Y, offsetof(Particle, pavg[0]), value, StructProperty::Float);
        value.Float = pavg1_val;
        sim->flood_prop(pos.X, pos.Y, offsetof(Particle, pavg[1]), value, StructProperty::Float);

		CloseActiveWindow();
		SelfDestruct();
	}});
	AddComponent(okayButton);
	SetOkayButton(okayButton);

	ui::Button * cancelButton = make_center_button(ui::Point(0, Size.Y - 16), ui::Point(Size.X / 2, 16), "Cancel");
    CANCEL_BUTTON(cancelButton);
    AddComponent(cancelButton);

	MakeActiveWindow();
}
