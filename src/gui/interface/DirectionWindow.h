#pragma once
#include <functional>

#include "gui/interface/Window.h"

namespace ui
{

	class DirectionSelector;
	class Label;

	class DirectionWindow : public ui::Window
	{
		void OnTryExit(ExitMethod method) override;
		void OnDraw() override;

		ui::DirectionSelector * direction;
		ui::Label * label;

	public:
		DirectionWindow(ui::Point position, float scale, int radius, String title, float x, float y, std::function<void(float, float)> callback);
	};

};
