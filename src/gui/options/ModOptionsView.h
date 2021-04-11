#ifndef MOPTIONSVIEW_H_
#define MOPTIONSVIEW_H_

#include "gui/interface/Window.h"
#include "gui/interface/ScrollPanel.h"
#include "gui/options/OptionsView.h"

namespace ui {
	class Checkbox;
	class DropDown;
	class DropDown;
}

class OptionsModel;
class OptionsController;
class ModOptionsView: public OptionsView {
	ui::Checkbox * autoOppositeTool;
	ui::Checkbox * secretModShortcut;
	ui::Checkbox * crosshairInBrush;
	ui::Checkbox * hollowBrushes;
	ui::Checkbox * autoHideHUD;
	ui::Checkbox * dimGlowMode;
	ui::DropDown * drawingFrqLimit;
	ui::Checkbox * fasterRenderer;
	ui::Checkbox * soundEnabled;

	ui::ScrollPanel * scrollPanel;
public:
	ModOptionsView();
	void NotifySettingsChanged(OptionsModel * sender);
	virtual ~ModOptionsView();
};

#endif /* MOPTIONSVIEW_H_ */
