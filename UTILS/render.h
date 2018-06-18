#pragma once
#include "../UTILS/interfaces.h"
#include "../SDK/ISurface.h"
#include "../SDK/IVDebugOverlay.h"
#include "../SDK/IEngine.h"

namespace SDK
{
	class IMaterial;
}

namespace RENDER
{
	unsigned int CreateF(std::string font_name, int size, int weight, int blur, int scanlines, int flags);
	void DrawF(int X, int Y, unsigned int Font, bool center_width, bool center_height, CColor Color, std::string Input);
	Vector2D GetTextSize(unsigned int Font, std::string Input);

	void DrawLine(int x1, int y1, int x2, int y2, CColor color);

	void DrawEmptyRect(int x1, int y1, int x2, int y2, CColor color, unsigned char = 0); // the flags are for which sides to ignore in clockwise, 0b1 is top, 0b10 is right, etc.
	void DrawFilledRect(int x1, int y1, int x2, int y2, CColor color);
	void DrawFilledRectOutline(int x1, int y1, int x2, int y2, CColor color);
	void DrawFilledRectArray(SDK::IntRect* rects, int rect_amount, CColor color);

	void DrawCircle(int x, int y, int radius, int segments, CColor color);
	void DrawFilledCircle(int x, int y, int radius, int segments, CColor color);

	void TexturedPolygon(int n, std::vector<SDK::Vertex_t> vertice, CColor color);
	void DrawSomething();

	__forceinline bool WorldToScreen(Vector& world, Vector& screen)
	{
		auto view_matrix = INTERFACES::Engine->GetMatrix();

		screen[0] = view_matrix[0][0] * world[0] + view_matrix[0][1] * world[1] + view_matrix[0][2] * world[2] + view_matrix[0][3];
		screen[1] = view_matrix[1][0] * world[0] + view_matrix[1][1] * world[1] + view_matrix[1][2] * world[2] + view_matrix[1][3];
		float w = view_matrix[3][0] * world[0] + view_matrix[3][1] * world[1] + view_matrix[3][2] * world[2] + view_matrix[3][3];

		if (w < 0.001f)
			return false;

		int width, height;
		INTERFACES::Engine->GetScreenSize(width, height);

		float invw = 1.0f / w;
		screen[0] *= invw;
		screen[1] *= invw;

		float x = static_cast<float>(width / 2);
		float y = static_cast<float>(height / 2);
		x += 0.5f * screen[0] * width + 0.5f;
		y -= 0.5f * screen[1] * height + 0.5f;
		screen[0] = x;
		screen[1] = y;

		return true;

		//return (INTERFACES::DebugOverlay->ScreenPosition(world, screen) != 1);
	}

	class RenderQueue
	{
	public:
		struct DrawCircleQueue
		{
			DrawCircleQueue(int x, int y, int radius, int segments, CColor color, float end_time = UTILS::GetCurtime() + 0.1f)
			{
				m_x = x;
				m_y = y;
				m_radius = radius;
				m_segments = segments;
				m_color = color;
				m_end_time = end_time;
			}

			int m_x;
			int m_y;
			int m_radius;
			int m_segments;
			float m_end_time;
			CColor m_color;
		};

		void Draw();
		void Clear();

		inline void AddDrawCircle(DrawCircleQueue circle)
		{
			circle_queue.push_back(circle);
		}

		inline void AddDrawCircle(Vector world_position, int radius, int segments, CColor color, float end_time = UTILS::GetCurtime() + 0.1f)
		{
			Vector screen;
			if (WorldToScreen(world_position, screen))
				circle_queue.push_back(DrawCircleQueue(screen.x, screen.y, radius, segments, color, end_time));
		}

	private:
		std::vector<DrawCircleQueue> circle_queue;
	};

	extern RenderQueue render_queue;
}
