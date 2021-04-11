#ifndef OPTIONSVIEW_H_
#define OPTIONSVIEW_H_

#include "gui/interface/Window.h"
#include "gui/interface/ScrollPanel.h"

namespace ui
{
	class Checkbox;
	class DropDown;
	class DropDown;
}

class OptionsModel;
class OptionsController;

class OptionsView: public ui::Window {
protected:
	OptionsController *c;
public:
	OptionsView() : ui::Window(ui::Point(-1, -1), ui::Point(340, 340)) {}
	virtual void NotifySettingsChanged(OptionsModel *sender) {};
	void AttachController(OptionsController *c_);
	void OnDraw() override;
	void OnTryExit(ExitMethod method) override;
	virtual ~OptionsView();
};

class VanillaOptionsView: public OptionsView
{
	ui::Checkbox * heatSimulation;
	ui::Checkbox * ambientHeatSimulation;
	ui::Checkbox * newtonianGravity;
	ui::Checkbox * waterEqualisation;
	ui::DropDown * airMode;
	ui::DropDown * gravityMode;
	ui::DropDown * edgeMode;
	ui::DropDown * scale;
	ui::Checkbox * resizable;
	ui::Checkbox * fullscreen;
	ui::Checkbox * altFullscreen;
	ui::Checkbox * forceIntegerScaling;
	ui::Checkbox * fastquit;
	ui::DropDown * decoSpace;
	ui::Checkbox * showAvatars;
	ui::Checkbox * momentumScroll;
	ui::Checkbox * autoDrawLimit;
	ui::Checkbox * mouseClickRequired;
	ui::Checkbox * includePressure;
	ui::Checkbox * perfectCirclePressure;

	ui::ScrollPanel * scrollPanel;
public:
	VanillaOptionsView();
	void NotifySettingsChanged(OptionsModel * sender);
	virtual ~VanillaOptionsView();
};

#endif /* OPTIONSVIEW_H_ */
