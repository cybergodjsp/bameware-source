#include "../includes.h"

#include "PPGUI_Defines.h"

namespace PPGUI
{
	HELPERS::PPGUI_Color ppgui_colors[PPGUI_Color_Names::PPGUI_COLOR_NUM];
	float ppgui_attributes[PPGUI_Attribute_Names::PPGUI_ATTRIBUTE_NUM];
	unsigned int ppgui_fonts[PPGUI_Font_Names::PPGUI_FONT_NUM];

	/// Command data base
	PPGUI_Command_Data_Base ppgui_command_data_base;

	/// PPGUI_Input
	void PPGUI_Input::Update()
	{
		/// Record previous input
		previous_mouse = real_current_mouse;
		memcpy(previous_keyboard, current_keyboard, 256 * sizeof(int));

		/// Mouse shitz
		const auto mouse_info = UTILS::INPUT::input_handler.GetMouseInfo();

		current_mouse.position = mouse_info.position;
		current_mouse.left_button = mouse_info.left;
		current_mouse.right_button = mouse_info.right;
		current_mouse.scroll = (mouse_info.scroll == 0) ? 0 : (mouse_info.scroll > 0 ? 1 : -1);

		/// Time between clicks, useful if u wanna use double click features or shit
		if (current_mouse.left_button && !previous_mouse.left_button)
		{
			click_time_delta = HELPERS::PPGUI_GetTime() - last_click_time;
			last_click_time = HELPERS::PPGUI_GetTime();
		}

		/// Time shit
		update_delta_time = HELPERS::PPGUI_GetTime() - last_update_time;
		last_update_time = HELPERS::PPGUI_GetTime();

		/// Update the keyboard
		UTILS::INPUT::input_handler.GetCurrentKeyboardState(current_keyboard);

		/// Copy to real shit
		real_previous_mouse = real_current_mouse;
		real_current_mouse = current_mouse;
	}
}