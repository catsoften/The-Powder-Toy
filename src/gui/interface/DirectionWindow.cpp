#include "DirectionWindow.h"

#include "gui/interface/Button.h"
#include "gui/interface/DirectionSelector.h"
#include "gui/interface/Label.h"
#include "gui/interface/Separator.h"
#include "gui/Style.h"

namespace ui
{

	void DirectionWindow::OnTryExit(ExitMethod method)
	{
		CloseActiveWindow();
		SelfDestruct();
	}

	void DirectionWindow::OnDraw()
	{
		Graphics * g = GetGraphics();

		g->DrawFilledRect(RectSized(Position - Vec2{ 1, 1 }, Size + Vec2{ 2, 2 }), 0x000000_rgb);
		g->DrawRect(RectSized(Position, Size), 0xC8C8C8_rgb);
	}

	DirectionWindow::DirectionWindow(ui::Point position, float scale, int radius, String title, float x, float y, std::function<void(float, float)> callback):
		ui::Window(position, ui::Point((radius * 5 / 2) + 20, (radius * 5 / 2) + 75)),
		direction(new ui::DirectionSelector(ui::Point(10, 32), scale, radius, radius / 4, 2, 5))
	{
		ui::Label * tempLabel = new ui::Label(ui::Point(4, 1), ui::Point(Size.X - 8, 22), title);
		tempLabel->SetTextColour(style::Colour::InformationTitle);
		tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
		tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
		AddComponent(tempLabel);

		auto * tempSeparator = new ui::Separator(ui::Point(0, 22), ui::Point(Size.X, 1));
		AddComponent(tempSeparator);

		label = new ui::Label(ui::Point(0, (radius * 5 / 2) + 37), ui::Point(Size.X, 16), String::Build(Format::Precision(1), "X:", x, " Y:", y, " Total:", std::hypot(x, y)));
		label->Appearance.HorizontalAlign = ui::Appearance::AlignCentre;
		label->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
		AddComponent(label);

		direction->SetValues(x, y);
		direction->SetUpdateCallback([this](float x, float y) {
			label->SetText(String::Build(Format::Precision(1), "X:", x, " Y:", y, " Total:", std::hypot(x, y)));
		});
		direction->SetSnapPoints(5, 5, 2 * scale / 0.05f);
		AddComponent(direction);

		ui::Button * okayButton = new ui::Button(ui::Point(0, Size.Y - 17), ui::Point(Size.X, 17), "OK");
		okayButton->Appearance.HorizontalAlign = ui::Appearance::AlignCentre;
		okayButton->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
		okayButton->Appearance.BorderInactive = ui::Colour(200, 200, 200);
		okayButton->SetActionCallback({ [this, callback] {
			callback(direction->GetXValue(), direction->GetYValue());
			CloseActiveWindow();
			SelfDestruct();
		} });
		AddComponent(okayButton);
		SetOkayButton(okayButton);

		MakeActiveWindow();
	}

};
