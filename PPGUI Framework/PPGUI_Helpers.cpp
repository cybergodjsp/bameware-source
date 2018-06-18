#include "../includes.h"

#include "../UTILS/render.h"
#include "../SDK/IEngine.h"

#include "PPGUI_Helpers.h"

namespace PPGUI
{
	namespace HELPERS
	{
		void PPGUI_DrawRectangle(PPGUI_Vector2D position, PPGUI_Vector2D size, PPGUI_Color color, bool outline)
		{
			if (outline)
				RENDER::DrawEmptyRect(position.x - 1, position.y - 1, position.x + size.x, position.y + size.y, PPGUI_Color::ToCColor(color));
			else
				RENDER::DrawFilledRect(position.x, position.y, position.x + size.x, position.y + size.y, PPGUI_Color::ToCColor(color));
		}
		void PPGUI_DrawText(PPGUI_Vector2D position, unsigned int font, bool center_width, bool center_height, PPGUI_Color color, const char* text)
		{
			RENDER::DrawF(position.x, position.y, font, center_width, center_height, PPGUI_Color::ToCColor(color), text);
		}
		void PPGUI_DrawLine(PPGUI_Vector2D position, PPGUI_Vector2D size, PPGUI_Color color)
		{
			RENDER::DrawLine(position.x, position.y, position.x + size.x, position.y + size.y, PPGUI_Color::ToCColor(color));
		}
		void PPGUI_DrawLineEx(PPGUI_Vector2D pos1, PPGUI_Vector2D pos2, PPGUI_Color color)
		{
			RENDER::DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, PPGUI_Color::ToCColor(color));
		}
		void PPGUI_DrawCircle(PPGUI_Vector2D position, PPGUI_Color color, int radius, int segments, bool filled)
		{
			if (filled)
				RENDER::DrawFilledCircle(position.x, position.y, radius, segments, PPGUI_Color::ToCColor(color));
			else
				RENDER::DrawCircle(position.x, position.y, radius, segments, PPGUI_Color::ToCColor(color));

		}
		int PPGUI_GetFilteredTextWidth(const char* name, int font)
		{
			char filtered_name[256];

			strcpy(filtered_name, name);
			PPGUI_FilterText(filtered_name);

			return PPGUI_GetTextSize(font, filtered_name).x;
		}

		PPGUI_Vector2D PPGUI_GetTextSize(unsigned int font, const char* text)
		{
			return RENDER::GetTextSize(font, text);
		}
		PPGUI_Vector2D PPGUI_GetScreenSize()
		{
			int width, height;
			INTERFACES::Engine->GetScreenSize(width, height);

			return PPGUI_Vector2D(width, height);
		}
	}
}