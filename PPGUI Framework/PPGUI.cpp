#include "../includes.h"

#include "PPGUI.h"

namespace PPGUI
{
	PPGUI_Framework menu;

	/// PPGUI_Frame
	PPGUI_Command_Frame::PPGUI_Command_Frame(PPGUI_Command* command, int columns)
	{
		const auto command_info = command->GetCommandInfo();

		m_command = command;

		m_parent_bounds_min = command_info.position, m_parent_bounds_max = command_info.position + command_info.size;
		const bool is_child_frame = menu.GetNumCommandFrames() > 0;
		if (is_child_frame)
			menu.GetCommandFrame().GetFrameBounds(m_parent_bounds_min, m_parent_bounds_max, HELPERS::CLAMP<int>(menu.GetColumn(), 0, columns - 1));

		/// Need to add the title bar height as padding for when the Command_Frame is a window
		auto title_bar_size = HELPERS::PPGUI_Vector2D(0, 0);
		if (command_info.command_type == PPGUI_Command_Types::PPGUI_COMMAND_TYPE_WINDOW)
			title_bar_size = HELPERS::PPGUI_Vector2D(0, ppgui_attributes[PPGUI_Attribute_Names::PPGUI_ATTRIBUTE_WINDOW_TITLE_BAR_HEIGHT]);

		const int size_per_column = command_info.size.x / columns;
		for (int i = 0; i < columns; i++)
		{
			auto column = PPGUI_Column();

			column.m_last_position = HELPERS::PPGUI_Vector2D(INT_MAX, INT_MAX), column.m_last_size = HELPERS::PPGUI_Vector2D(INT_MAX, INT_MAX);
			column.m_did_scroll = false;

			column.m_frame_min = command_info.position + HELPERS::PPGUI_Vector2D(size_per_column * i, 0) + title_bar_size;
			column.m_frame_max = command_info.position + HELPERS::PPGUI_Vector2D(size_per_column * (i + 1), command_info.size.y);

			column.m_bounds_min = column.m_frame_min;
			column.m_bounds_max = column.m_frame_max;

			if (is_child_frame)
			{
				if (column.m_bounds_min.y < m_parent_bounds_min.y)
					column.m_bounds_min.y = m_parent_bounds_min.y;
				if (column.m_bounds_max.y > m_parent_bounds_max.y)
					column.m_bounds_max.y = m_parent_bounds_max.y;
			}

			column.m_original_bounds_min = column.m_bounds_min;
			column.m_original_bounds_max = column.m_bounds_max;

			m_columns.push_back(column);
		}
	}
	bool PPGUI_Command_Frame::AllocateSpace(HELPERS::PPGUI_Vector2D size, HELPERS::PPGUI_Vector2D &position, bool &scroll_enabled, bool same_line)
	{
		auto &column = m_columns[HELPERS::CLAMP<int>(menu.GetColumn(), 0, m_columns.size() - 1)];
		auto command_info = m_command->GetCommandInfo();

		/// Ideal position 
		auto padding = HELPERS::PPGUI_Vector2D(ppgui_attributes[PPGUI_ATTRIBUTE_PADDING_WIDTH], ppgui_attributes[PPGUI_ATTRIBUTE_PADDING_HEIGHT]);
		auto ideal_position = column.m_frame_min + padding;

		/// Was AllocateSpace() already called for this frame
		const bool already_called = column.m_last_position.x != INT_MAX && column.m_last_position.y != INT_MAX;

		/// Attach to last command shit
		const bool should_attach = menu.ShouldAttachToLastCommand() && already_called;
		const bool was_previous_attached = menu.WasLastCommandAttached() && already_called;
		if (should_attach)
		{
			menu.SetShouldAttachToLastCommand(false);
			same_line = true;
			scroll_enabled = column.m_did_scroll;
		}
		menu.SetWasLastCommandAttached(should_attach);

		/// Scroll when it's the first command in the frame
		if (column.m_did_scroll)
			scroll_enabled = true;

		if (scroll_enabled && !already_called)
			ideal_position.y += m_command->GetScrollAmount();

		/// If already called AllocateSpace() on another command
		if (already_called)
		{
			ideal_position.y = column.m_last_position.y;

			if (scroll_enabled && !column.m_did_scroll)
				ideal_position.y += m_command->GetScrollAmount();

			if (same_line)
			{
				if (should_attach)
				{
					if (was_previous_attached)
						ideal_position.x = column.m_last_position.x - padding.x - size.x;
					else
						ideal_position.x = column.m_bounds_max.x - padding.x - size.x;
				}
				else
					ideal_position.x = column.m_last_position.x + column.m_last_size.x + padding.x;
			}
			else
				ideal_position.y += column.m_last_size.y + padding.y;
		}

		if (scroll_enabled)
			column.m_did_scroll = true;
		if (!scroll_enabled)
		{
			const int new_bounds_min = (ideal_position.y + size.y + (padding.y * 0.5f));
			if (new_bounds_min <= column.m_bounds_max.y && new_bounds_min >= column.m_bounds_min.y)
				column.m_bounds_min.y = new_bounds_min;
		}

		position = ideal_position;

		column.m_last_position = position;
		column.m_last_size.x = size.x;
		if (!same_line || column.m_last_size.y < size.y || !already_called)
			column.m_last_size.y = size.y;

		if (menu.GetColumn() == -1)
		{
			for (auto &clm : m_columns)
			{
				clm.m_did_scroll = column.m_did_scroll;
				clm.m_bounds_min.y = column.m_bounds_min.y;
				clm.m_last_size = column.m_last_size;

				clm.m_last_position.y = column.m_last_position.y;
				clm.m_last_position.x = clm.m_frame_min.x + padding.x;
			}
		}

		return true;
	}

	/// PPGUI_Framework
	void PPGUI_Framework::BeginFrame()
	{
		m_input.Update();
	}
	void PPGUI_Framework::EndFrame()
	{
		if (m_command_frames.size())
			throw std::exception("Failed to match a Begin() with an End()");

		for (const auto &command : m_end_commands)
			m_command_list.Insert(command);

		m_end_commands.clear();

		const auto commands = m_command_list.Get();

		/// Test input for clicks and stuff
		for (const auto &command : commands)
			command->TestInput(m_input);

		/// Traverse the commands list backwards when drawing so the last added command is drawn on top of everything else
		for (int i = commands.size() - 1; i >= 0; i--)
			commands[i]->Draw();

		/// Draw mouse
		if (m_should_draw_mouse)
			DrawMouse();

		/// Reset shit
		m_command_list.Clear();
		m_last_command = nullptr;
		m_should_draw_mouse = false;
	}
	void PPGUI_Framework::DrawMouse()
	{
		const auto pos = m_input.current_mouse.position;

		for (int i = 0; i <= 9; i++)
			HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x, pos.y), HELPERS::PPGUI_Vector2D(pos.x + i, pos.y + 11), HELPERS::PPGUI_Color::White());
		for (int i = 0; i <= 7; i++)
			HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x, pos.y + 9 + i), HELPERS::PPGUI_Vector2D(pos.x + i, pos.y + 9), HELPERS::PPGUI_Color::White());
		for (int i = 0; i <= 3; i++)
			HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x + 6 + i, pos.y + 11), HELPERS::PPGUI_Vector2D(pos.x, pos.y + i), HELPERS::PPGUI_Color::White());

		HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x + 5, pos.y + 11), HELPERS::PPGUI_Vector2D(pos.x + 8, pos.y + 18), HELPERS::PPGUI_Color::White());
		HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x + 4, pos.y + 11), HELPERS::PPGUI_Vector2D(pos.x + 7, pos.y + 18), HELPERS::PPGUI_Color::White());

		HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x, pos.y), HELPERS::PPGUI_Vector2D(pos.x, pos.y + 17), HELPERS::PPGUI_Color::Black());
		HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x, pos.y + 17), HELPERS::PPGUI_Vector2D(pos.x + 3, pos.y + 14), HELPERS::PPGUI_Color::Black());

		HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x + 4, pos.y + 14), HELPERS::PPGUI_Vector2D(pos.x + 7, pos.y + 19), HELPERS::PPGUI_Color::Black());
		HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x + 5, pos.y + 14), HELPERS::PPGUI_Vector2D(pos.x + 7, pos.y + 19), HELPERS::PPGUI_Color::Black());

		HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x + 7, pos.y + 18), HELPERS::PPGUI_Vector2D(pos.x + 9, pos.y + 18), HELPERS::PPGUI_Color::Black());

		HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x + 10, pos.y + 18), HELPERS::PPGUI_Vector2D(pos.x + 7, pos.y + 12), HELPERS::PPGUI_Color::Black());

		HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x + 7, pos.y + 12), HELPERS::PPGUI_Vector2D(pos.x + 12, pos.y + 12), HELPERS::PPGUI_Color::Black());

		HELPERS::PPGUI_DrawLineEx(HELPERS::PPGUI_Vector2D(pos.x + 12, pos.y + 12), HELPERS::PPGUI_Vector2D(pos.x, pos.y), HELPERS::PPGUI_Color::Black());
	}
	bool PPGUI_Framework::AllocateSpace(HELPERS::PPGUI_Vector2D size, HELPERS::PPGUI_Vector2D &position, bool &scroll_enabled)
	{
		if (m_should_same_line)
		{
			m_should_same_line = false;
			return GetCommandFrame().AllocateSpace(size, position, scroll_enabled, true);
		}
		else
			return GetCommandFrame().AllocateSpace(size, position, scroll_enabled, false);
	}
	PPGUI_Command_Frame& PPGUI_Framework::GetCommandFrame()
	{
		if (!m_command_frames.size())
			throw std::exception("Not inside a command frame");

		return m_command_frames.front();
	}

	/// Wrappers for class methods and shit
	void PPGUI_BeginWindow(const char* name, HELPERS::PPGUI_Vector2D position, HELPERS::PPGUI_Vector2D size, int columns)
	{
		auto command = new PPGUI_Window_Command(name, position, size);

		command->GetCommandInfo().space = size;

		menu.AddCommand(command);
		menu.AddCommandFrame(PPGUI_Command_Frame(command, columns));
	}
	void PPGUI_EndWindow()
	{
		auto command_frame = menu.GetCommandFrame();
		auto command = command_frame.GetCommand();
		if (!command || command->GetCommandInfo().command_type != PPGUI_Command_Types::PPGUI_COMMAND_TYPE_WINDOW)
			throw std::exception("BeginWindow() improperly ended");

		menu.RemoveCommandFrame();
	}

	/// Checkboxes
	bool PPGUI_Checkbox(const char* name, bool &variable, bool scroll_enabled)
	{
		/// Get the checkbox width and height from the ppgui_attributes[]
		const int width = ppgui_attributes[PPGUI_Attribute_Names::PPGUI_ATTRIBUTE_CHECKBOX_WIDTH],
			height = ppgui_attributes[PPGUI_Attribute_Names::PPGUI_ATTRIBUTE_CHECKBOX_HEIGHT];

		const unsigned int font = ppgui_fonts[PPGUI_FONT_CHECKBOX];
		const int text_width = HELPERS::PPGUI_GetFilteredTextWidth(name, font);

		/// size is the size of the checkbox, occupied_size is the size + the text width
		HELPERS::PPGUI_Vector2D	position, size(width, height), 
			occupied_size(width + (text_width > 0 ? text_width + 5 : 0), height);
		if (!menu.AllocateSpace(occupied_size, position, scroll_enabled))
			return variable;

		auto command = new PPGUI_Checkbox_Command(name, variable, position, size, scroll_enabled);

		command->GetCommandInfo().space = occupied_size;

		menu.AddCommand(command);

		return variable;
	}

	/// Buttons
	bool PPGUI_Button(const char* name, bool scroll_enabled)
	{
		/// Get the button width and height from the ppgui_attributes[]
		const int width = ppgui_attributes[PPGUI_Attribute_Names::PPGUI_ATTRIBUTE_BUTTON_WIDTH],
			height = ppgui_attributes[PPGUI_Attribute_Names::PPGUI_ATTRIBUTE_BUTTON_HEIGHT];

		HELPERS::PPGUI_Vector2D position, size(width, height);
		if (!menu.AllocateSpace(size, position, scroll_enabled))
			return false;

		auto command = new PPGUI_Button_Command(name, position, size, scroll_enabled);

		command->GetCommandInfo().space = size;

		menu.AddCommand(command);

		return ppgui_command_data_base.Get<PPGUI_Command_Data_Base::PPGUI_Button_Command_Data>(command->GetDataHash(), PPGUI_Command_Types::PPGUI_COMMAND_TYPE_BUTTON).was_clicked;
	}

	/// Groupboxes
	void PPGUI_BeginGroupbox(const char* name, int height, int columns, bool scroll_enabled)
	{
		const int padding = ppgui_attributes[PPGUI_ATTRIBUTE_PADDING_WIDTH];
		HELPERS::PPGUI_Vector2D position, size(menu.GetCurrentFrameSize().x - (padding * 2), height);

		if (!menu.AllocateSpace(size, position, scroll_enabled))
			return;

		auto command = new PPGUI_Groupbox_Command(name, position, size, scroll_enabled);

		command->GetCommandInfo().space = size;

		menu.AddCommand(command);
		menu.AddCommandFrame(PPGUI_Command_Frame(command, columns));
	}
	void PPGUI_EndGroupbox()
	{
		auto command = menu.GetCommandFrame().GetCommand();
		if (!command || command->GetCommandInfo().command_type != PPGUI_Command_Types::PPGUI_COMMAND_TYPE_GROUPBOX)
			throw std::exception("BeginGroupbox() improperly ended");

		menu.RemoveCommandFrame();
	}

	/// Sliders
	float PPGUI_Slider(const char* name, const char* suffix, float min, float max, float &variable, int decimal_precision, bool scroll_enabled)
	{
		const int width = ppgui_attributes[PPGUI_Attribute_Names::PPGUI_ATTRIBUTE_SLIDER_WIDTH],
			height = ppgui_attributes[PPGUI_Attribute_Names::PPGUI_ATTRIBUTE_SLIDER_HEIGHT];
		const unsigned int font = ppgui_fonts[PPGUI_FONT_SLIDER_NAME];

		HELPERS::PPGUI_Vector2D position, size(width, height),
			occupied_size = size + HELPERS::PPGUI_Vector2D(HELPERS::PPGUI_GetFilteredTextWidth(name, font) + 5, 0);
		if (!menu.AllocateSpace(occupied_size, position, scroll_enabled))
			return variable;

		auto command = new PPGUI_Slider_Command(name, suffix, min, max, variable, decimal_precision, scroll_enabled, position, size);

		command->GetCommandInfo().space = occupied_size;

		menu.AddCommand(command);

		return variable;
	}

	/// Separators
	void PPGUI_Separator(const char* name, bool scroll_enabled)
	{
		const int padding = ppgui_attributes[PPGUI_Attribute_Names::PPGUI_ATTRIBUTE_PADDING_WIDTH];

		HELPERS::PPGUI_Vector2D position, size(menu.GetCurrentFrameSize().x - (padding * 2), 0);
		if (!menu.AllocateSpace(size, position, scroll_enabled))
			return;

		auto command = new PPGUI_Separator_Command(name, position, size, scroll_enabled);
		menu.AddCommand(command);
	}

	/// Keybinds
	int PPGUI_Keybind(const char* name, int &variable, bool scroll_enabled)
	{
		const int width = ppgui_attributes[PPGUI_ATTRIBUTE_KEYBIND_WIDTH], 
			height = ppgui_attributes[PPGUI_ATTRIBUTE_KEYBIND_HEIGHT];
		const unsigned int font = ppgui_fonts[PPGUI_FONT_KEYBIND_NAME];

		HELPERS::PPGUI_Vector2D position, size(width, height),
			occupied_size = size + HELPERS::PPGUI_Vector2D(HELPERS::PPGUI_GetFilteredTextWidth(name, font) + 5, 0);
		if (!menu.AllocateSpace(occupied_size, position, scroll_enabled))
			return variable;

		auto command = new PPGUI_Keybind_Command(name, variable, position, size, scroll_enabled);

		command->GetCommandInfo().space = occupied_size;

		menu.AddCommand(command);

		return variable;
	}

	/// Text Input
	const char* PPGUI_TextInput(const char* name, char* variable, bool scroll_enabled)
	{
		const unsigned int font = ppgui_fonts[PPGUI_FONT_TEXT_INPUT_NAME];
		const int width = ppgui_attributes[PPGUI_ATTRIBUTE_TEXT_INPUT_WIDTH], 
			height = ppgui_attributes[PPGUI_ATTRIBUTE_TEXT_INPUT_HEIGHT], 
			text_width = HELPERS::PPGUI_GetFilteredTextWidth(name, font);

		HELPERS::PPGUI_Vector2D position, size(width, height),
			occupied_size = size + HELPERS::PPGUI_Vector2D(text_width > 0 ? text_width + 5 : 0, 0);
		if (!menu.AllocateSpace(occupied_size, position, scroll_enabled))
			return variable;

		auto command = new PPGUI_Text_Input_Command(name, position, size, variable, scroll_enabled);

		command->GetCommandInfo().space = occupied_size;

		menu.AddCommand(command);

		return variable;
	}

	/// Comboboxes
	int PPGUI_Combobox(const char* name, int items_amount, char (*items)[PPGUI_MAX_STRING_LENGTH], int &variable, bool scroll_enabled)
	{
		const unsigned int font = ppgui_fonts[PPGUI_FONT_COMBOBOX_NAME];
		const int width = ppgui_attributes[PPGUI_ATTRIBUTE_COMBOBOX_WIDTH],
			height = ppgui_attributes[PPGUI_ATTRIBUTE_COMBOBOX_HEIGHT],
			text_width = HELPERS::PPGUI_GetFilteredTextWidth(name, font);

		HELPERS::PPGUI_Vector2D position, size(width, height), 
			occupied_size = size + HELPERS::PPGUI_Vector2D(text_width > 0 ? text_width + 5 : 0, 0);

		if (!menu.AllocateSpace(occupied_size, position, scroll_enabled))
			return variable;

		auto command = new PPGUI_Combobox_Command(name, position, size, items, items_amount, variable, scroll_enabled);
		command->GetCommandInfo().space = occupied_size;
		menu.AddCommandToEnd(command);

		return variable;
	}

	/// Multiselect comboboxes
	void PPGUI_MultiselectCombobox(const char* name, int items_amount, char(*items)[PPGUI_MAX_STRING_LENGTH], bool* selected_items, bool scroll_enabled)
	{
		const unsigned int font = ppgui_fonts[PPGUI_FONT_COMBOBOX_NAME];
		const int width = ppgui_attributes[PPGUI_ATTRIBUTE_COMBOBOX_WIDTH],
			height = ppgui_attributes[PPGUI_ATTRIBUTE_COMBOBOX_HEIGHT];

		HELPERS::PPGUI_Vector2D position, size(width, height);
		if (!menu.AllocateSpace(size, position, scroll_enabled))
			return;

		auto command = new PPGUI_Multiselect_Combobox_Command(name, position, size, items, selected_items, items_amount, scroll_enabled);
		command->GetCommandInfo().space = size;
		menu.AddCommandToEnd(command);
	}

	/// Color pickers
	void PPGUI_ColorPicker(const char* name, unsigned char &R, unsigned char &G, unsigned char &B, unsigned char &A, bool scroll_enabled)
	{
		const int width = ppgui_attributes[PPGUI_ATTRIBUTE_COLOR_PICKER_WIDTH], 
			height = ppgui_attributes[PPGUI_ATTRIBUTE_COLOR_PICKER_HEIGHT];
		const unsigned int font = ppgui_fonts[PPGUI_FONT_COLOR_PICKER];
		const int text_width = HELPERS::PPGUI_GetFilteredTextWidth(name, font);

		HELPERS::PPGUI_Vector2D position, size(width, height), 
			occupied_size(width + (text_width > 0 ? text_width + 5 : 0), height);
		if (!menu.AllocateSpace(occupied_size, position, scroll_enabled))
			return;

		auto command = new PPGUI_Color_Picker_Command(name, position, size, R, G, B, A, scroll_enabled);

		command->GetCommandInfo().space = occupied_size;

		menu.AddCommandToEnd(command);
	}

	/// Tabs
	int PPGUI_Tab(int tabs_amount, char(*tabs)[PPGUI_MAX_STRING_LENGTH], int &variable, bool scroll_enabled)
	{
		const int padding = ppgui_attributes[PPGUI_ATTRIBUTE_PADDING_WIDTH];
		const int width = menu.GetCurrentFrameSize().x - (padding * 2), 
			height = ppgui_attributes[PPGUI_ATTRIBUTE_TAB_HEIGHT];

		HELPERS::PPGUI_Vector2D position, size(width, height);
		if (!menu.AllocateSpace(size, position, scroll_enabled))
			return variable;

		auto command = new PPGUI_Tab_Command(position, size, variable, tabs, tabs_amount, scroll_enabled);

		command->GetCommandInfo().space = size;

		menu.AddCommand(command);

		return variable;
	}

	/// Tool tips
	void PPGUI_ToolTip(const char* description, int lines_amount, HELPERS::PPGUI_Vector2D offset)
	{
		const auto last_command = menu.GetLastCommand();
		if (!last_command)
			return;

		auto command = new PPGUI_Tool_Tip_Command(description, lines_amount, offset, last_command);
		menu.AddCommandToEnd(command, true);
	}



	void PPGUI_Demo()
	{
		PPGUI_DrawMouse();

		PPGUI_BeginWindow("PPGUI Demo Window", HELPERS::PPGUI_Vector2D(400, 500), HELPERS::PPGUI_Vector2D(600, 400));
		{
			static float alpha = 1.f;

			static bool checkbox_bool = false;
			PPGUI_AlphaModulate(alpha);
			PPGUI_Checkbox("PPGUI Demo Checkbox", checkbox_bool);

			PPGUI_AlphaModulate(alpha);
			if (PPGUI_Button("PPGUI Button"))
				alpha = fmod(alpha + 0.1f, 1.f);

			PPGUI_BeginGroupbox("PPGUI Demo Groupbox", 300);
			{
				static float slider_float = 0.f;
				PPGUI_AlphaModulate(alpha);
				PPGUI_Slider("PPGUI Demo Slider", "%", 0.f, 100.f, slider_float, false);

				PPGUI_AlphaModulate(alpha);
				PPGUI_Separator("PPGUI Demo Separator", false);

				static int keybind_int = -1;
				PPGUI_AlphaModulate(alpha);
				PPGUI_Keybind("PPGUI Demo Keybind", keybind_int);

				static char text_input_string[PPGUI_MAX_STRING_LENGTH];
				PPGUI_AlphaModulate(alpha);
				PPGUI_TextInput("PPGUI Demo Text Input", text_input_string);

				static char combobox_items[][PPGUI_MAX_STRING_LENGTH] =
				{
					"Item 1",
					"Item 2",
					"Item 3",
					"Item 4",
					"Item 5",
					"Item 6"
				};
				static int combobox_selected_item = 0;
				PPGUI_AlphaModulate(alpha);
				PPGUI_Combobox("PPGUI Demo Combobox", 6, combobox_items, combobox_selected_item);

				static bool multiselect_selected_items[6];
				PPGUI_AlphaModulate(alpha);
				PPGUI_MultiselectCombobox("PPGUI Demo Multiselect Combobox", 6, combobox_items, multiselect_selected_items);

				static unsigned char R = 255, G = 255, B = 255, A = 255;
				PPGUI_AttachToPreviousWidget();
				PPGUI_AlphaModulate(alpha);
				PPGUI_ColorPicker("PPGUI Demo Colorpicker", R, G, B, A);

				static char tab_items[][PPGUI_MAX_STRING_LENGTH] =
				{
					"Tab 1",
					"Tab 2",
					"Tab 3"
				};
				static int tab_selected_item = 0;
				PPGUI_AlphaModulate(alpha);
				PPGUI_Tab(3, tab_items, tab_selected_item);

				PPGUI_ToolTip("I am a PPGUI Demo Tab");
			} PPGUI_EndGroupbox();
		} PPGUI_EndWindow();
	}
}