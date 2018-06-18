#pragma once

/*
global variables and other stuff that is probably too small to put in another 
file (and i'm too lazy to make files for all of them) would go in here.
Stuff like fonts and shit
*/

namespace GLOBAL
{
	extern HWND csgo_hwnd;

	extern bool should_send_packet;
	extern bool disable_fake_lag;
	extern bool is_fakewalking;
	extern bool should_choke_packets;
	extern bool is_freestanding;
	extern bool is_edging;
	extern bool can_shoot_someone;
	extern bool is_jumping;
	extern bool do_max_fake_lag;
	extern int choke_amount;
	extern float cheat_start_time;

	extern float testing_float_1;
	extern float testing_float_2;
	extern bool testing_bool_1;
	extern bool testing_bool_2;

	extern void* last_cmd;

	extern Vector real_angles;
	extern Vector fake_angles;

	extern CColor synced_rainbow_color;
}

namespace FONTS
{
	extern unsigned int menu_tab_font;
	extern unsigned int menu_checkbox_font;
	extern unsigned int menu_slider_font;
	extern unsigned int menu_slider_value_font;
	extern unsigned int menu_groupbox_font;
	extern unsigned int menu_combobox_name_font;
	extern unsigned int menu_combobox_value_font;
	extern unsigned int menu_window_font;
	extern unsigned int menu_separator_font;
	extern unsigned int menu_window_blurr_font;
	extern unsigned int menu_keybind_value_font;
	extern unsigned int menu_keybind_name_font;
	extern unsigned int menu_text_input_name_font;
	extern unsigned int menu_text_input_value_font;
	extern unsigned int menu_button_font;
	extern unsigned int menu_colorpicker_font;
	extern unsigned int menu_tool_tip_font;

	extern unsigned int visuals_esp_font;
	extern unsigned int lby_indicator_font;
	extern unsigned int in_game_logging_font;
	extern unsigned int weapon_icon_font;
	extern unsigned int welcome_font;

	bool ShouldReloadFonts();
	void InitFonts();
}
