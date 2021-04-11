#include "DecorationTool.h"

#include "graphics/Renderer.h"

#include "simulation/SimulationData.h"
#include "simulation/Simulation.h"
#include "simulation/ElementCommon.h"

VideoBuffer *DecorationTool::GetIcon(int toolID, int width, int height)
{
	float lightness = std::max(std::max(Red, Green), Blue) / 2 + std::min(std::min(Red, Green), Blue) / 2;
	int text_color_bw = lightness < 255 / 3 ? 240 : 50;

	VideoBuffer * newTexture = new VideoBuffer(width, height);
	for (int y=0; y<height; y++)
	{
		for (int x=0; x<width; x++)
		{
			//if (toolID == DECO_LIGH)
			//	vid_buf[WINDOWW*(y+j)+(x+i)] = PIXRGB(PIXR(pc)-10*j, PIXG(pc)-10*j, PIXB(pc)-10*j);
			//else if (toolID == DECO_DARK)
			//	vid_buf[WINDOWW*(y+j)+(x+i)] = PIXRGB(PIXR(pc)+10*j, PIXG(pc)+10*j, PIXB(pc)+10*j);
			if (toolID == DECO_SMUDGE)
				newTexture->SetPixel(x, y, 0, 255-5*x, 5*x, 255);
			else if (toolID == DECO_DRAW || toolID == DECO_CLEAR)
				newTexture->SetPixel(x, y, Red, Green, Blue, Alpha);
			else if (toolID == DECO_NOISE) {
				// 2 px border for noise
				if (x < 2 || x >= width - 2 || y < 2 || y >= height - 2 || (x / 2 + y / 2) % 2 == 1) {
					newTexture->SetPixel(x, y, Red, Green, Blue, Alpha);
					continue;
				}
				newTexture->SetPixel(x, y, text_color_bw, text_color_bw, text_color_bw, 255);
			}
			else if (toolID == DECO_OVERLAY) {
				bool in_box1 = x >= 2 && x < 15 && y >= 2 && y < 10;
				bool in_box2 = x >= 10 && x <= width - 2 && y >= 5 && y <= height - 2;
				if (in_box1 && in_box2)
					newTexture->SetPixel(x, y, 128, 128, 128, 255);
				else if (in_box1)
					newTexture->SetPixel(x, y, Red, Green, Blue, 255);
				else if (in_box2)
					newTexture->SetPixel(x, y, 255 - Red, 255 - Green, 255 - Blue, 255);
				else
					newTexture->SetPixel(x, y, text_color_bw, text_color_bw, text_color_bw, 255);
			}
			
			else if (toolID == DECO_DODGE || toolID == DECO_BURN)
				newTexture->SetPixel(x, y, Red, Green, Blue, 255);
			else
				newTexture->SetPixel(x, y, text_color_bw, text_color_bw, text_color_bw, 255);
		}
	}
	if (toolID == DECO_CLEAR)
	{
		int reverseRed = (Red + 127) % 256;
		int reverseGreen = (Green + 127) % 256;
		int reverseBlue = (Blue + 127) % 256;

		for (int y=4; y<12; y++)
		{
			newTexture->SetPixel(y+5, y-1, reverseRed, reverseGreen, reverseBlue, 255);
			newTexture->SetPixel(y+6, y-1, reverseRed, reverseGreen, reverseBlue, 255);
			newTexture->SetPixel(20-y, y-1, reverseRed, reverseGreen, reverseBlue, 255);
			newTexture->SetPixel(21-y, y-1, reverseRed, reverseGreen, reverseBlue, 255);
		}
	}
	else if (toolID == DECO_ADD)
		newTexture->BlendCharacter(11, 4, '+', Red, Green, Blue, 255);
	else if (toolID == DECO_SUBTRACT)
		newTexture->BlendCharacter(11, 4, '-', Red, Green, Blue, 255);
	else if (toolID == DECO_MULTIPLY)
		newTexture->BlendCharacter(11, 3, 'x', Red, Green, Blue, 255);
	else if (toolID == DECO_DIVIDE)
		newTexture->BlendCharacter(11, 4, '/', Red, Green, Blue, 255);

	else if (toolID == DECO_SPONGE) {
		newTexture->BlendCharacter(5, 4, 'S', Red, Green, Blue, 255);
		newTexture->BlendCharacter(11, 4, '-', Red, Green, Blue, 255);
		newTexture->BlendCharacter(17, 4, '-', Red, Green, Blue, 255);
	}
	else if (toolID == DECO_SATURATE) {
		newTexture->BlendCharacter(5, 4, 'S', Red, Green, Blue, 255);
		newTexture->BlendCharacter(11, 4, '+', Red, Green, Blue, 255);
		newTexture->BlendCharacter(17, 4, '+', Red, Green, Blue, 255);
	}
	else if (toolID == DECO_BURN) {
		newTexture->BlendCharacter(5,  4, 'L', text_color_bw, text_color_bw, text_color_bw, 255);
		newTexture->BlendCharacter(11, 4, '-', text_color_bw, text_color_bw, text_color_bw, 255);
		newTexture->BlendCharacter(17, 4, '-', text_color_bw, text_color_bw, text_color_bw, 255);
	}
	else if (toolID == DECO_DODGE) {
		newTexture->BlendCharacter(5,  4, 'L', text_color_bw, text_color_bw, text_color_bw, 255);
		newTexture->BlendCharacter(11, 4, '+', text_color_bw, text_color_bw, text_color_bw, 255);
		newTexture->BlendCharacter(17, 4, '+', text_color_bw, text_color_bw, text_color_bw, 255);
	}
	else if (toolID == DECO_MULTIPLY2)
		newTexture->BlendCharacter(10, 4, 'M', Red, Green, Blue, 255);
	else if (toolID == DECO_SCREEN)
		newTexture->BlendCharacter(11, 4, 'S', Red, Green, Blue, 255);
	return newTexture;
}

DecorationTool::DecorationTool(Renderer *ren_, int decoMode, String name, String description, int r, int g, int b, ByteString identifier):
	Tool(decoMode, name, description, r, g, b, identifier),
	Red(0),
	Green(0),
	Blue(0),
	Alpha(0),
	ren(ren_)
{
}

DecorationTool::~DecorationTool()
{
}

void DecorationTool::Draw(Simulation * sim, Brush * brush, ui::Point position)
{
	sim->ApplyDecorationPoint(position.X, position.Y, Red, Green, Blue, Alpha, toolID, brush);
}

void DecorationTool::DrawLine(Simulation * sim, Brush * brush, ui::Point position1, ui::Point position2, bool dragging)
{
	sim->ApplyDecorationLine(position1.X, position1.Y, position2.X, position2.Y, Red, Green, Blue, Alpha, toolID, brush);
}

void DecorationTool::DrawRect(Simulation * sim, Brush * brush, ui::Point position1, ui::Point position2)
{
	sim->ApplyDecorationBox(position1.X, position1.Y, position2.X, position2.Y, Red, Green, Blue, Alpha, toolID);
}

void DecorationTool::DrawFill(Simulation * sim, Brush * brush, ui::Point position)
{
	pixel loc = ren->vid[position.X+position.Y*WINDOWW];
	if (toolID == DECO_CLEAR)
		sim->ApplyDecorationFill(ren, position.X, position.Y, 0, 0, 0, 0, PIXR(loc), PIXG(loc), PIXB(loc));
	else
		sim->ApplyDecorationFill(ren, position.X, position.Y, Red, Green, Blue, Alpha, PIXR(loc), PIXG(loc), PIXB(loc));
}

