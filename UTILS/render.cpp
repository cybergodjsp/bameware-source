#include "../includes.h"

#include "render.h"
#include "interfaces.h"

#include "../SDK/IMaterial.h"
#include "../SDK/CGlobalVars.h"

namespace RENDER
{
	unsigned int CreateF(std::string font_name, int size, int weight, int blur, int scanlines, int flags)
	{
		auto font = INTERFACES::Surface->SCreateFont();
		INTERFACES::Surface->SetFontGlyphSet(font, font_name.c_str(), size, weight, blur, scanlines, flags);

		return font;
	}

	void DrawF(int X, int Y, unsigned int Font, bool center_width, bool center_height, CColor Color, std::string Input)
	{
		/* char -> wchar */
		size_t size = Input.size() + 1;
		auto wide_buffer = std::make_unique<wchar_t[]>(size);
		mbstowcs_s(0, wide_buffer.get(), size, Input.c_str(), size - 1);

		/* check center */
		int width = 0, height = 0;
		INTERFACES::Surface->GetTextSize(Font, wide_buffer.get(), width, height);
		if (!center_width)
			width = 0;
		if (!center_height)
			height = 0;

		/* call and draw*/
		INTERFACES::Surface->DrawSetTextColor(Color);
		INTERFACES::Surface->DrawSetTextFont(Font);
		INTERFACES::Surface->DrawSetTextPos(X - (width * .5), Y - (height * .5));
		INTERFACES::Surface->DrawPrintText(wide_buffer.get(), wcslen(wide_buffer.get()), 0);
	}

	Vector2D GetTextSize(unsigned int Font, std::string Input)
	{
		/* char -> wchar */
		size_t size = Input.size() + 1;
		auto wide_buffer = std::make_unique<wchar_t[]>(size);
		mbstowcs_s(0, wide_buffer.get(), size, Input.c_str(), size - 1);

		int width, height;
		INTERFACES::Surface->GetTextSize(Font, wide_buffer.get(), width, height);

		return Vector2D(width, height);
	}

	void DrawLine(int x1, int y1, int x2, int y2, CColor color)
	{
		INTERFACES::Surface->DrawSetColor(color);
		INTERFACES::Surface->DrawLine(x1, y1, x2, y2);
	}

	void DrawEmptyRect(int x1, int y1, int x2, int y2, CColor color, unsigned char ignore_flags)
	{
		INTERFACES::Surface->DrawSetColor(color);
		if (!(ignore_flags & 0b1))
			INTERFACES::Surface->DrawLine(x1, y1, x2, y1);
		if (!(ignore_flags & 0b10))
			INTERFACES::Surface->DrawLine(x2, y1, x2, y2);
		if (!(ignore_flags & 0b100))
			INTERFACES::Surface->DrawLine(x2, y2, x1, y2);
		if (!(ignore_flags & 0b1000))
			INTERFACES::Surface->DrawLine(x1, y2, x1, y1);
	}

	void DrawFilledRect(int x1, int y1, int x2, int y2, CColor color)
	{
		INTERFACES::Surface->DrawSetColor(color);
		INTERFACES::Surface->DrawFilledRect(x1, y1, x2, y2);
	}

	void DrawFilledRectOutline(int x1, int y1, int x2, int y2, CColor color)
	{
		INTERFACES::Surface->DrawSetColor(color);
		INTERFACES::Surface->DrawFilledRect(x1, y1, x2, y2);
		DrawEmptyRect(x1 - 1, y1 - 1, x2, y2, CColor(0, 0, 0, 100));
	}

	void DrawFilledRectArray(SDK::IntRect* rects, int rect_amount, CColor color)
	{
		INTERFACES::Surface->DrawSetColor(color);
		INTERFACES::Surface->DrawFilledRectArray(rects, rect_amount);
	}

	void DrawCircle(int x, int y, int radius, int segments, CColor color)
	{
		INTERFACES::Surface->DrawSetColor(color);
		INTERFACES::Surface->DrawOutlinedCircle(x, y, radius, segments);
	}

	void TexturedPolygon(int n, std::vector<SDK::Vertex_t> vertice, CColor color)
	{
		static int texture_id = INTERFACES::Surface->CreateNewTextureID(true); // 
		static unsigned char buf[4] = {255, 255, 255, 255};
		INTERFACES::Surface->DrawSetTextureRGBA(texture_id, buf, 1, 1); //
		INTERFACES::Surface->DrawSetColor(color); //
		INTERFACES::Surface->DrawSetTexture(texture_id); //
		INTERFACES::Surface->DrawTexturedPolygon(n, vertice.data()); //
	}

	void DrawFilledCircle(int x, int y, int radius, int segments, CColor color)
	{
		std::vector<SDK::Vertex_t> vertices;
		float step = M_PI * 2.0f / segments;
		for (float a = 0; a < (M_PI * 2.0f); a += step)
			vertices.push_back(SDK::Vertex_t(Vector2D(radius * cosf(a) + x, radius * sinf(a) + y)));

		TexturedPolygon(vertices.size(), vertices, color);
	}

	void DrawSomething() { }

	RenderQueue render_queue;

	void RenderQueue::Draw()
	{
		for (const auto& circle : circle_queue)
		{
			RENDER::DrawCircle(circle.m_x, circle.m_y, circle.m_radius, circle.m_segments, circle.m_color);
		}
	}

	void RenderQueue::Clear()
	{
		for (int i = circle_queue.size() - 1; i >= 0; i--)
		{
			if (circle_queue[i].m_end_time < UTILS::GetCurtime())
				circle_queue.erase(circle_queue.begin() + i);
		}
	}
}
