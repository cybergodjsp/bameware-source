#pragma once

#include "PPGUI_Defines.h"

namespace PPGUI
{
	/// Base class that all commands inherit from
	class PPGUI_Command
	{
	public:
		virtual ~PPGUI_Command() {};

		virtual void Draw() {};
		virtual void TestInput(PPGUI_Input &input) {};

		void SetupCommandInfo(PPGUI_Command_Info& info, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, const char* name, bool scroll_enabled);
		uint32_t GetDataHash();

		/// Scale from 0 to 1
		float GetScrollAlphaScale()
		{
			if (!(m_command_info.flags & PPGUI_COMMAND_FLAG_SCROLL_ENABLED))
				return 1.f;

			constexpr float alpha_fade_pixel_distance = 5.f;
			return HELPERS::CLAMP(HELPERS::MIN<float>(
				(m_command_info.position.y - m_command_info.bounds_min.y) / alpha_fade_pixel_distance,
				(m_command_info.bounds_max.y - (m_command_info.position.y + m_command_info.size.y)) / 
				alpha_fade_pixel_distance), 0.25f, 1.f);
		}
		float GetFadeAlphaScale()
		{
			return 0.f;
		}

		/// Returns false if nothing is in bounds
		bool GetClampedPositionAndSize(HELPERS::PPGUI_Vector2D &position, HELPERS::PPGUI_Vector2D &size)
		{
			const HELPERS::PPGUI_Vector2D pixel(1, 1);
			const bool is_scroll_enabled = m_command_info.flags & PPGUI_COMMAND_FLAG_SCROLL_ENABLED;

			if (m_command_info.column == -1)
			{
				if (!HELPERS::PPGUI_Vector2D::ClampToBounds(position, size, m_command_info.entire_bounds_min + pixel, m_command_info.entire_bounds_max - pixel))
					return false;
				else
					return true;
			}
			else
			{
				if (is_scroll_enabled && !HELPERS::PPGUI_Vector2D::ClampToBounds(position, size, m_command_info.bounds_min + pixel, m_command_info.bounds_max - pixel))
					return false;
				if (!is_scroll_enabled && !HELPERS::PPGUI_Vector2D::ClampToBounds(position, size, m_command_info.original_bounds_min + pixel, m_command_info.original_bounds_max - pixel))
					return false;
			}

			return true;
		}

		/// Returns whether the rectangle is ENTIRELY in bounds
		static bool IsRectInBounds(PPGUI_Command* command, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size)
		{
			const auto command_info = command->GetCommandInfo();

			const bool is_scroll_enabled = command_info.flags & PPGUI_COMMAND_FLAG_SCROLL_ENABLED;

			if (command_info.column == -1)
			{
				return position.IsPointInBounds(command_info.entire_bounds_min, command_info.entire_bounds_max) &&
					(position + size).IsPointInBounds(command_info.entire_bounds_min, command_info.entire_bounds_max);
			}
			else
			{
				if (is_scroll_enabled)
					return position.IsPointInBounds(command_info.bounds_min, command_info.bounds_max) &&
					(position + size).IsPointInBounds(command_info.bounds_min, command_info.bounds_max);
				else
					return position.IsPointInBounds(command_info.original_bounds_min, command_info.original_bounds_max) &&
					(position + size).IsPointInBounds(command_info.original_bounds_min, command_info.original_bounds_max);
			}
		}
		bool IsRectInBounds(HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size)
		{
			return IsRectInBounds(this, position, size);
		}

		/// Returns whether the point is in bounds
		bool IsPointInBounds(HELPERS::PPGUI_Vector2D position)
		{
			const bool is_scroll_enabled = m_command_info.flags & PPGUI_COMMAND_FLAG_SCROLL_ENABLED;

			if (m_command_info.column == -1)
				return position.IsPointInBounds(m_command_info.entire_bounds_min, m_command_info.entire_bounds_max);
			else
			{
				if (is_scroll_enabled)
					return position.IsPointInBounds(m_command_info.bounds_min, m_command_info.bounds_max);
				else
					return position.IsPointInBounds(m_command_info.original_bounds_min, m_command_info.original_bounds_max);
			}
		}

		PPGUI_Command_Info& GetCommandInfo() { return m_command_info; }

		virtual int GetScrollAmount() { return 0; };

	private:
		PPGUI_Command_Info m_command_info;
	};

	class PPGUI_Window_Command : public PPGUI_Command
	{
	public:
		PPGUI_Window_Command(const char* name, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size);

		void Draw();
		void TestInput(PPGUI_Input &input);

		int GetScrollAmount();
	};

	class PPGUI_Checkbox_Command : public PPGUI_Command
	{
	public:
		PPGUI_Checkbox_Command(const char* name, bool &variable, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, bool scroll_enabled);

		void Draw();
		void TestInput(PPGUI_Input &input);

	private:
		bool *m_variable_ptr = nullptr;
	};

	class PPGUI_Button_Command : public PPGUI_Command
	{
	public:
		PPGUI_Button_Command(const char* name, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, bool scroll_enabled);

		void Draw();
		void TestInput(PPGUI_Input &input);
	};

	class PPGUI_Groupbox_Command : public PPGUI_Command
	{
	public:
		PPGUI_Groupbox_Command(const char* name, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, bool scroll_enabled);

		void Draw();
		void TestInput(PPGUI_Input &input);

		int GetScrollAmount();

	private:
	};

	class PPGUI_Slider_Command : public PPGUI_Command
	{
	public:
		PPGUI_Slider_Command(const char* name, const char* suffix, float min, float max, float &variable, int decimal_precision, bool scroll_enabled, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size);

		void Draw();
		void TestInput(PPGUI_Input &input);

	private:
		char m_suffix[16];
		float *m_variable_ptr = nullptr;
		float m_min, m_max;
		int m_decimal_precision;
	};

	class PPGUI_Separator_Command : public PPGUI_Command
	{
	public:
		PPGUI_Separator_Command(const char* name, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, bool scroll_enabled);

		void Draw();
	};

	class PPGUI_Keybind_Command : public PPGUI_Command
	{
	public:
		PPGUI_Keybind_Command(const char* name, int &variable, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, bool scroll_enabled);

		void Draw();
		void TestInput(PPGUI_Input &input);

	private:
		int *m_variable_ptr = nullptr;
	};

	class PPGUI_Text_Input_Command : public PPGUI_Command
	{
	public:
		PPGUI_Text_Input_Command(const char* name, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, char* variable, bool scroll_enabled);

		void Draw();
		void TestInput(PPGUI_Input &input);

	private:
		char* m_variable_ptr = nullptr;
	};

	class PPGUI_Combobox_Command : public PPGUI_Command
	{
	public:
		PPGUI_Combobox_Command(const char* name, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, char (*items)[PPGUI_MAX_STRING_LENGTH], int items_amount, int &variable, bool scroll_enabled);

		void Draw();
		void TestInput(PPGUI_Input &input);

	private:
		int* m_variable_ptr = nullptr;
		char (*m_items)[PPGUI_MAX_STRING_LENGTH];
		int m_items_amount;
	};

	class PPGUI_Multiselect_Combobox_Command : public PPGUI_Command
	{
	public:
		PPGUI_Multiselect_Combobox_Command(const char* name, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, char(*items)[PPGUI_MAX_STRING_LENGTH], bool* selected_items, int items_amount, bool scroll_enabled);

		void Draw();
		void TestInput(PPGUI_Input& input);

	private:
		bool *m_selected_items = nullptr;
		char(*m_items)[PPGUI_MAX_STRING_LENGTH];
		int m_items_amount;
	};

	class PPGUI_Color_Picker_Command : public PPGUI_Command
	{
	public:
		PPGUI_Color_Picker_Command(const char* name, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, unsigned char &R, unsigned char &G, unsigned char &B, unsigned char &A, bool scroll_enabled);

		void Draw();
		void TestInput(PPGUI_Input &input);

	private:
		unsigned char* m_red_ptr = nullptr, *m_green_ptr = nullptr, 
			*m_blue_ptr = nullptr, *m_alpha_ptr = nullptr;
	};

	class PPGUI_Tab_Command : public PPGUI_Command
	{
	public:
		PPGUI_Tab_Command(HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, int &variable, char(*tabs)[PPGUI_MAX_STRING_LENGTH], int tabs_amount, bool scroll_enabled);

		void Draw();
		void TestInput(PPGUI_Input &input);

	private:
		int m_tabs_amount;
		int *m_variable_ptr = nullptr;
		char(*m_tabs)[PPGUI_MAX_STRING_LENGTH];
	};

	class PPGUI_Tool_Tip_Command : public PPGUI_Command
	{
	public:
		PPGUI_Tool_Tip_Command(const char* description, int lines_amount, HELPERS::PPGUI_Vector2D offset, PPGUI_Command* parent_command);

		void Draw();
		void TestInput(PPGUI_Input &input);

	private:
		PPGUI_Command* m_parent_command = nullptr;
		int m_lines_amount = 0;
	};
}