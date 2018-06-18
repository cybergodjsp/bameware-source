#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../UTILS/hooks.h"
#include "../SDK/IEngine.h"
#include "../SDK/CInputSystem.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseEntity.h"

#include "../FEATURES/SkinChanger.h"
#include "../FEATURES/World Textures.h"
#include "../FEATURES/Configurations.h"

#include "Menu.h"

namespace PPGUI
{
	namespace MENU
	{
		void Do()
		{
			auto SetColor = []() -> void
			{
				const auto theme_color = HELPERS::PPGUI_Color::HotPink();

				ppgui_colors[PPGUI_COLOR_WINDOW_BODY] = HELPERS::PPGUI_Color(40, 40, 40, 255);
				ppgui_colors[PPGUI_COLOR_WINDOW_BODY_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_WINDOW_TITLE_BAR] = HELPERS::PPGUI_Color(50, 50, 50, 255);
				ppgui_colors[PPGUI_COLOR_WINDOW_TITLE_BAR_TEXT] = theme_color;
				ppgui_colors[PPGUI_COLOR_WINDOW_TITLE_BAR_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_WINDOW_TITLE_BAR_BACKGROUND_TEXT] = HELPERS::PPGUI_Color(19, 160, 216, 30);

				ppgui_colors[PPGUI_COLOR_CHECKBOX_UNSELECTED_BODY] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_CHECKBOX_UNSELECTED_BODY_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 150);
				ppgui_colors[PPGUI_COLOR_CHECKBOX_SELECTED_BODY] = theme_color;
				ppgui_colors[PPGUI_COLOR_CHECKBOX_SELECTED_BODY_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 150);
				ppgui_colors[PPGUI_COLOR_CHECKBOX_TEXT] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_CHECKBOX_HOVERED_TEXT] = HELPERS::PPGUI_Color::White();

				ppgui_colors[PPGUI_COLOR_BUTTON_BODY] = HELPERS::PPGUI_Color(80, 80, 80, 255);
				ppgui_colors[PPGUI_COLOR_BUTTON_BODY_CLICKED] = HELPERS::PPGUI_Color(100, 100, 100, 255);
				ppgui_colors[PPGUI_COLOR_BUTTON_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 150);
				ppgui_colors[PPGUI_COLOR_BUTTON_TEXT] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_BUTTON_TEXT_CLICKED] = theme_color;

				ppgui_colors[PPGUI_COLOR_GROUPBOX_BODY] = HELPERS::PPGUI_Color(50, 50, 50, 255);
				ppgui_colors[PPGUI_COLOR_GROUPBOX_BODY_HOVERED] = HELPERS::PPGUI_Color(50, 50, 50, 255);
				ppgui_colors[PPGUI_COLOR_GROUPBOX_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 150);
				ppgui_colors[PPGUI_COLOR_GROUPBOX_TEXT] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_GROUPBOX_TEXT_HOVERED] = HELPERS::PPGUI_Color::White();

				ppgui_colors[PPGUI_COLOR_SLIDER_BODY] = HELPERS::PPGUI_Color(80, 80, 80, 255);
				ppgui_colors[PPGUI_COLOR_SLIDER_BODY_SELECTED] = theme_color;
				ppgui_colors[PPGUI_COLOR_SLIDER_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 150);
				ppgui_colors[PPGUI_COLOR_SLIDER_TEXT_NAME] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_SLIDER_TEXT_NAME_HOVERED] = HELPERS::PPGUI_Color::White();
				ppgui_colors[PPGUI_COLOR_SLIDER_VALUE_TEXT] = HELPERS::PPGUI_Color::White();

				ppgui_colors[PPGUI_COLOR_SEPARATOR_BODY] = HELPERS::PPGUI_Color(100, 100, 100, 255);
				ppgui_colors[PPGUI_COLOR_SEPARATOR_TEXT] = HELPERS::PPGUI_Color::White();

				ppgui_colors[PPGUI_COLOR_KEYBIND_BODY] = HELPERS::PPGUI_Color(80, 80, 80, 255);
				ppgui_colors[PPGUI_COLOR_KEYBIND_BODY_SELECTED] = HELPERS::PPGUI_Color(110, 110, 110, 255);
				ppgui_colors[PPGUI_COLOR_KEYBIND_OUTLINE ] = HELPERS::PPGUI_Color(0, 0, 0, 150);
				ppgui_colors[PPGUI_COLOR_KEYBIND_TEXT_NAME] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_KEYBIND_TEXT_NAME_HOVERED] = HELPERS::PPGUI_Color::White();
				ppgui_colors[PPGUI_COLOR_KEYBIND_TEXT_VALUE] = HELPERS::PPGUI_Color::White();

				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_BODY] = HELPERS::PPGUI_Color(80, 80, 80, 255);
				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_BODY_SELECTED] = HELPERS::PPGUI_Color(110, 110, 110, 255);
				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 150);
				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_TEXT_VALUE] = HELPERS::PPGUI_Color::White();
				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_TEXT_NAME] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_TEXT_HOVERED] = HELPERS::PPGUI_Color::White();

				ppgui_colors[PPGUI_COLOR_COMBOBOX_BODY_SELECTED] = HELPERS::PPGUI_Color(80, 80, 80, 255);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_OUTLINE_SELECTED] = HELPERS::PPGUI_Color(0, 0, 0, 150);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_BODY_ITEM] = HELPERS::PPGUI_Color(50, 50, 50, 255);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_OUTLINE_ITEM] = HELPERS::PPGUI_Color(0, 0, 0, 150);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_SELECTED_TEXT_ITEM] = theme_color;
				ppgui_colors[PPGUI_COLOR_COMBOBOX_TEXT_ITEM] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_TEXT_SELECTED] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_TEXT_SELECTED_HOVERED] = HELPERS::PPGUI_Color::White();
				ppgui_colors[PPGUI_COLOR_COMBOBOX_TEXT_NAME] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_TEXT_NAME_HOVERED] = HELPERS::PPGUI_Color::White();

				ppgui_colors[PPGUI_COLOR_COLOR_PICKER_BODY_COLOR] = HELPERS::PPGUI_Color(50, 50, 50, 255);
				ppgui_colors[PPGUI_COLOR_COLOR_PICKER_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 150);
				ppgui_colors[PPGUI_COLOR_COLOR_PICKER_TEXT] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_COLOR_PICKER_TEXT_HOVERED] = HELPERS::PPGUI_Color::White();

				ppgui_colors[PPGUI_COLOR_TAB_BODY] = HELPERS::PPGUI_Color(40, 40, 40, 255);
				ppgui_colors[PPGUI_COLOR_TAB_BODY_SELECTED] = HELPERS::PPGUI_Color(70, 70, 70, 255);
				ppgui_colors[PPGUI_COLOR_TAB_TEXT] = HELPERS::PPGUI_Color(255, 255, 255, 255);
				ppgui_colors[PPGUI_COLOR_TAB_TEXT_SELECTED] = theme_color;
				ppgui_colors[PPGUI_COLOR_TAB_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 150);

				ppgui_colors[PPGUI_COLOR_TOOL_TIP_BODY] = HELPERS::PPGUI_Color(200, 200, 200, 255);
				ppgui_colors[PPGUI_COLOR_TOOL_TIP_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 255);
				ppgui_colors[PPGUI_COLOR_TOOL_TIP_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 255);
			};
			auto SetAttributes = []() -> void
			{
				ppgui_attributes[PPGUI_ATTRIBUTE_PADDING_HEIGHT] = 12;
				ppgui_attributes[PPGUI_ATTRIBUTE_PADDING_WIDTH] = 12;
				ppgui_attributes[PPGUI_ATTRIBUTE_SCROLL_SPEED] = 10;

				ppgui_attributes[PPGUI_ATTRIBUTE_WINDOW_TITLE_BAR_HEIGHT] = 30;

				ppgui_attributes[PPGUI_ATTRIBUTE_CHECKBOX_HEIGHT] = 10;
				ppgui_attributes[PPGUI_ATTRIBUTE_CHECKBOX_WIDTH] = 10;

				ppgui_attributes[PPGUI_ATTRIBUTE_BUTTON_HEIGHT] = 16;
				ppgui_attributes[PPGUI_ATTRIBUTE_BUTTON_WIDTH] = 100;
				ppgui_attributes[PPGUI_ATTRIBUTE_BUTTON_CLICK_DURATION] = 0.15f;

				ppgui_attributes[PPGUI_ATTRIBUTE_SLIDER_HEIGHT] = 6;
				ppgui_attributes[PPGUI_ATTRIBUTE_SLIDER_WIDTH] = 200;

				ppgui_attributes[PPGUI_ATTRIBUTE_KEYBIND_HEIGHT] = 14;
				ppgui_attributes[PPGUI_ATTRIBUTE_KEYBIND_WIDTH] = 100;

				ppgui_attributes[PPGUI_ATTRIBUTE_TEXT_INPUT_HEIGHT] = 14;
				ppgui_attributes[PPGUI_ATTRIBUTE_TEXT_INPUT_WIDTH] = 200;

				ppgui_attributes[PPGUI_ATTRIBUTE_COMBOBOX_WIDTH] = 200;
				ppgui_attributes[PPGUI_ATTRIBUTE_COMBOBOX_HEIGHT] = 14;
				ppgui_attributes[PPGUI_ATTRIBUTE_COMBOBOX_MAX_ITEMS_DISPLAYED] = 8;

				ppgui_attributes[PPGUI_ATTRIBUTE_COLOR_PICKER_WIDTH] = 30;
				ppgui_attributes[PPGUI_ATTRIBUTE_COLOR_PICKER_HEIGHT] = 10;
				ppgui_attributes[PPGUI_ATTRIBUTE_COLOR_PICKER_OPEN_WIDTH] = 250;
				ppgui_attributes[PPGUI_ATTRIBUTE_COLOR_PICKER_OPEN_HEIGHT] = 150;
				ppgui_attributes[PPGUI_ATTRIBUTE_COLOR_PICKER_SLIDER_WIDTH] = 14;
				ppgui_attributes[PPGUI_ATTRIBUTE_COLOR_PICKER_PADDING] = 8;
				ppgui_attributes[PPGUI_ATTRIBUTE_COLOR_PICKER_PIXELATION] = 6;

				ppgui_attributes[PPGUI_ATTRIBUTE_TAB_HEIGHT] = 16;
				ppgui_attributes[PPGUI_ATTRIBUTE_TAB_ITEM_PADDING] = 10;

				ppgui_attributes[PPGUI_ATTRIBUTE_TOOL_TIP_LINE_HEIGHT] = 16;
			};
			auto SetFonts = []()
			{
				ppgui_fonts[PPGUI_FONT_WINDOW_NAME] = FONTS::menu_window_font;
				ppgui_fonts[PPGUI_FONT_WINDOW_NAME_BEHIND] = FONTS::menu_window_blurr_font;

				ppgui_fonts[PPGUI_FONT_GROUPBOX] = FONTS::menu_groupbox_font;

				ppgui_fonts[PPGUI_FONT_SEPARATOR] = FONTS::menu_separator_font;

				ppgui_fonts[PPGUI_FONT_CHECKBOX] = FONTS::menu_checkbox_font;

				ppgui_fonts[PPGUI_FONT_BUTTON] = FONTS::menu_button_font;

				ppgui_fonts[PPGUI_FONT_SLIDER_NAME] = FONTS::menu_slider_font;
				ppgui_fonts[PPGUI_FONT_SLIDER_VALUE] = FONTS::menu_slider_value_font;

				ppgui_fonts[PPGUI_FONT_KEYBIND_NAME] = FONTS::menu_keybind_name_font;
				ppgui_fonts[PPGUI_FONT_KEYBIND_VALUE] = FONTS::menu_keybind_value_font;

				ppgui_fonts[PPGUI_FONT_TEXT_INPUT_NAME] = FONTS::menu_text_input_name_font;
				ppgui_fonts[PPGUI_FONT_TEXT_INPUT_VALUE] = FONTS::menu_text_input_value_font;

				ppgui_fonts[PPGUI_FONT_COMBOBOX_NAME] = FONTS::menu_combobox_name_font;
				ppgui_fonts[PPGUI_FONT_COMBOBOX_SELECTED_ITEM] = FONTS::menu_combobox_value_font;
				ppgui_fonts[PPGUI_FONT_COMBOBOX_ITEM_NAME] = FONTS::menu_combobox_value_font;

				ppgui_fonts[PPGUI_FONT_TAB_SELECTED_ITEM_NAME] = FONTS::menu_tab_font;
				ppgui_fonts[PPGUI_FONT_TAB_ITEM_NAME] = FONTS::menu_tab_font;

				ppgui_fonts[PPGUI_FONT_TOOL_TIP] = FONTS::menu_tool_tip_font;

				ppgui_fonts[PPGUI_FONT_COLOR_PICKER] = FONTS::menu_colorpicker_font;
			};
			auto Uninject = []() -> void
			{
				HMODULE h_module;
				if (GetModuleHandleEx(NULL, NULL, &h_module) != 0)
				{
					INTERFACES::Engine->ClientCmd("cl_mouseenable 1");
					INTERFACES::InputSystem->EnableInput(true);
					HOOKS::UnHook();
					FreeLibrary(h_module);

					DllMain(h_module, DLL_PROCESS_DETACH, NULL);
				}
			};

			auto SetDebugColor = []()
			{
				ppgui_colors[PPGUI_COLOR_WINDOW_BODY] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_WINDOW_BODY_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_WINDOW_TITLE_BAR] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_WINDOW_TITLE_BAR_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_WINDOW_TITLE_BAR_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_WINDOW_TITLE_BAR_BACKGROUND_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 0);

				ppgui_colors[PPGUI_COLOR_CHECKBOX_UNSELECTED_BODY] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_CHECKBOX_UNSELECTED_BODY_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_CHECKBOX_SELECTED_BODY] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_CHECKBOX_SELECTED_BODY_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_CHECKBOX_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_CHECKBOX_HOVERED_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 0);

				ppgui_colors[PPGUI_COLOR_BUTTON_BODY] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_BUTTON_BODY_CLICKED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_BUTTON_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_BUTTON_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_BUTTON_TEXT_CLICKED] = HELPERS::PPGUI_Color(0, 0, 0, 0);

				ppgui_colors[PPGUI_COLOR_GROUPBOX_BODY] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_GROUPBOX_BODY_HOVERED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_GROUPBOX_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_GROUPBOX_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_GROUPBOX_TEXT_HOVERED] = HELPERS::PPGUI_Color(0, 0, 0, 0);

				ppgui_colors[PPGUI_COLOR_SLIDER_BODY] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_SLIDER_BODY_SELECTED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_SLIDER_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_SLIDER_TEXT_NAME] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_SLIDER_TEXT_NAME_HOVERED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_SLIDER_VALUE_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 0);

				ppgui_colors[PPGUI_COLOR_SEPARATOR_BODY] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_SEPARATOR_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 0);

				ppgui_colors[PPGUI_COLOR_KEYBIND_BODY] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_KEYBIND_BODY_SELECTED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_KEYBIND_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_KEYBIND_TEXT_NAME] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_KEYBIND_TEXT_NAME_HOVERED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_KEYBIND_TEXT_VALUE] = HELPERS::PPGUI_Color(0, 0, 0, 0);

				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_BODY] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_BODY_SELECTED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_TEXT_VALUE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_TEXT_NAME] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_TEXT_INPUT_TEXT_HOVERED] = HELPERS::PPGUI_Color(0, 0, 0, 0);

				ppgui_colors[PPGUI_COLOR_COMBOBOX_BODY_SELECTED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_OUTLINE_SELECTED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_BODY_ITEM] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_OUTLINE_ITEM] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_SELECTED_TEXT_ITEM] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_TEXT_ITEM] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_TEXT_SELECTED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_TEXT_SELECTED_HOVERED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_TEXT_NAME] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COMBOBOX_TEXT_NAME_HOVERED] = HELPERS::PPGUI_Color(0, 0, 0, 0);

				ppgui_colors[PPGUI_COLOR_COLOR_PICKER_BODY_COLOR] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COLOR_PICKER_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COLOR_PICKER_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_COLOR_PICKER_TEXT_HOVERED] = HELPERS::PPGUI_Color(0, 0, 0, 0);

				ppgui_colors[PPGUI_COLOR_TAB_BODY] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_TAB_BODY_SELECTED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_TAB_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_TAB_TEXT_SELECTED] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_TAB_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);

				ppgui_colors[PPGUI_COLOR_TOOL_TIP_BODY] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_TOOL_TIP_OUTLINE] = HELPERS::PPGUI_Color(0, 0, 0, 0);
				ppgui_colors[PPGUI_COLOR_TOOL_TIP_TEXT] = HELPERS::PPGUI_Color(0, 0, 0, 0);
			};

			///SetDebugColor();
			SetColor();
			SetAttributes();
			SetFonts();


			/// Start
			PPGUI_BeginFrame();

			/// Menu toggle
			static bool menu_open = false;
			if (menu.GetInput().DidClickKey(VK_INSERT))
			{
				menu_open = !menu_open;

				INTERFACES::Engine->ClientCmd(menu_open ? "cl_mouseenable 0" : "cl_mouseenable 1");
				INTERFACES::InputSystem->EnableInput(!menu_open);
			}

			if (!menu_open)
			{
				PPGUI_EndFrame();
				return;
			}

			/// Draw mouse
			PPGUI_DrawMouse();

			/// Skin changer
			PPGUI_BeginWindow("Skins", HELPERS::PPGUI_Vector2D(1100, 200), HELPERS::PPGUI_Vector2D(400, 230));
			{
				PPGUI_SetColumn(1);

				static int selected_weapon = 0;
				PPGUI_Combobox("Weapons", FEATURES::MISC::skin_changer.NUM_WEAPONS, FEATURES::MISC::skin_changer.weapons_list, selected_weapon, false);

				PPGUI_Combobox("Skin", FEATURES::MISC::skin_changer.NUM_SKINS, FEATURES::MISC::skin_changer.skins_list, SETTINGS::skin_configs.weapon_skin[selected_weapon], false);

				PPGUI_Combobox("Quality", FEATURES::MISC::skin_changer.NUM_QUALITIES, FEATURES::MISC::skin_changer.qualities_list, SETTINGS::skin_configs.weapon_quality[selected_weapon], false);

				PPGUI_Slider("Wear", "", 0.f, 1.f, SETTINGS::skin_configs.weapon_wear[selected_weapon], 2, false);

				PPGUI_Slider("Seed", "", 0.f, 1000.f, SETTINGS::skin_configs.weapon_seed[selected_weapon], 0, false);

				if (PPGUI_Checkbox("StatTrak", SETTINGS::skin_configs.weapon_stat_trak_enabled[selected_weapon], false))
				{
					PPGUI_SameLine();
					PPGUI_Slider("Kills", "", 0.f, 99999.f, SETTINGS::skin_configs.weapon_stat_trak_kills[selected_weapon], 0, false);
				}

				PPGUI_TextInput("Custom name", SETTINGS::skin_configs.weapon_custom_name[selected_weapon], false);

				if (PPGUI_Button("Update", false))
					INTERFACES::Engine->ForceFullUpdate();

				PPGUI_SameLine();
				if (PPGUI_Button("Save", false))
					SETTINGS::skin_configs.Save("skins_config.scfg", sizeof(SETTINGS::SkinConfig));

				PPGUI_SameLine();
				if (PPGUI_Button("Load", false))
				{
					SETTINGS::skin_configs.Load("skins_config.scfg", sizeof(SETTINGS::SkinConfig));
					INTERFACES::Engine->ForceFullUpdate();
				}
			} PPGUI_EndWindow();

#ifdef DEV_MODE
			/*{
				PPGUI_BeginWindow("Dev Window", HELPERS::PPGUI_Vector2D(1100, 500), HELPERS::PPGUI_Vector2D(400, 230));
				{
					PPGUI_SetColumn(1);

					PPGUI_Slider("Test float 1", "", -1000, 1000, GLOBAL::testing_float_1);
					PPGUI_Slider("Test float 2", "", -1000, 1000, GLOBAL::testing_float_2);
					PPGUI_Checkbox("Test bool 1", GLOBAL::testing_bool_1);
					PPGUI_Checkbox("Test bool 2", GLOBAL::testing_bool_2);
					if (PPGUI_Button("Test button 1"))
					{
						const auto address = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
						std::cout << std::hex << address << std::endl;
						std::cout << address << std::endl;
					}
				} PPGUI_EndWindow();
			}*/
#endif

			/// Main window
			PPGUI_BeginWindow("Bameware", HELPERS::PPGUI_Vector2D(200, 200), HELPERS::PPGUI_Vector2D(800, 450), 2);
			{
				/// Main tabs
				static char main_tabs[][PPGUI_MAX_STRING_LENGTH] =
				{
					"Ragebot",
					"Legitbot",
					"Visuals",
					"Misc"
				};

				static int selected_main_tab = 0;
				PPGUI_SetColumn(0);
				switch (PPGUI_Tab(4, main_tabs, selected_main_tab, false))
				{
				case 0: /// Ragebot
				{					
					PPGUI_SetColumn(1);

					PPGUI_BeginGroupbox("Aimbot", 260);
					{
						if (PPGUI_Checkbox("Enabled##aimbot enabled", SETTINGS::ragebot_configs.aimbot_enabled, false))
						{
							PPGUI_SameLine();
							PPGUI_Checkbox("Autostop", SETTINGS::ragebot_configs.aimbot_autostop_enabled, false);

							PPGUI_Checkbox("Prefer bodyaim", SETTINGS::ragebot_configs.aimbot_prefer_baim_enabled, false);

							PPGUI_SameLine();
							PPGUI_Checkbox("Bodyaim if unplugged", SETTINGS::ragebot_configs.aimbot_baim_if_unplugged, false);

							PPGUI_Slider("Bodyaim if x damage", "dmg", 0.f, 100.f, SETTINGS::ragebot_configs.aimbot_bodyaim_if_x_damage, 0, false);
							PPGUI_Slider("Point-scale", "%", 0.f, 100.f, SETTINGS::ragebot_configs.aimbot_pointscale, 0, false);
							PPGUI_Slider("Body point-scale", "%", 0.f, 100.f, SETTINGS::ragebot_configs.aimbot_hitscan_pointscale, 0, false);
							PPGUI_Slider("Min damage", "dmg", 5.f, 110.f, SETTINGS::ragebot_configs.aimbot_minimum_damage, 0, false);
							PPGUI_Slider("Min autowall damage", "dmg", 5.f, 110.f, SETTINGS::ragebot_configs.aimbot_minimum_autowall_damage, 0, false);
							PPGUI_Slider("First shot delay", "s", 0.f, 0.2f, SETTINGS::ragebot_configs.aimbot_delay_shot, 2, false);

							static char accuracy_types[][PPGUI_MAX_STRING_LENGTH] =
							{
								"Disabled",
								"Hitchance",
								"Spread limit"
							};
							PPGUI_Combobox("Accuracy type", 3, accuracy_types, SETTINGS::ragebot_configs.aimbot_accuracy_type, false);

							if (!SETTINGS::ragebot_configs.aimbot_accuracy_type)
							{
								PPGUI_AlphaModulate(0.2f);
								PPGUI_Disable();
								PPGUI_Slider("##accuracy value aimbot", "", 0.f, 100.f, SETTINGS::ragebot_configs.aimbot_accuracy, 0, false);
							}

							if (SETTINGS::ragebot_configs.aimbot_accuracy_type == 1)
								PPGUI_Slider("##accuracy value aimbot", "%", 0.f, 100.f, SETTINGS::ragebot_configs.aimbot_accuracy, 0, false);
							else if (SETTINGS::ragebot_configs.aimbot_accuracy_type == 2)
								PPGUI_Slider("##accuracy value aimbot", "", 0.f, 0.1f, SETTINGS::ragebot_configs.aimbot_accuracy, 3, false);

							static char hitscan_items[][PPGUI_MAX_STRING_LENGTH] =
							{
								"Chest",
								"Stomach",
								"Arms",
								"Legs",
								"Feet"
							};
							PPGUI_MultiselectCombobox("Hitscan groups", 5, hitscan_items, SETTINGS::ragebot_configs.aimbot_selected_hitgroups, false);
							PPGUI_ToolTip("Hitbox groups to hitscan.");

							PPGUI_MultiselectCombobox("Hitscan multipoint groups", 4, hitscan_items, SETTINGS::ragebot_configs.aimbot_selected_multipoint_hitgroups, false);
							PPGUI_ToolTip("Hitbox groups to hitscan, will be multipointed.");
						}
					} PPGUI_EndGroupbox();


					PPGUI_SetColumn(2);

					PPGUI_BeginGroupbox("Anti-aim", 200);
					{
						if (PPGUI_Checkbox("Enabled", SETTINGS::ragebot_configs.antiaim_enabled, false))
						{
							static char antiaim_tabs[][PPGUI_MAX_STRING_LENGTH] =
							{
								"Standing",
								"Moving",
								"In-air",
								"Edging"
							};
							static char antiaim_real_types[][PPGUI_MAX_STRING_LENGTH] =
							{
								"Viewangle",
								"Static",
								"Lowerbody",
								"Adaptive",
								"Spin"
							};
							static char antiaim_fake_types[][PPGUI_MAX_STRING_LENGTH] =
							{
								"Viewangle",
								"Static",
								"Lowerbody",
								"Adaptive",
								"Spin",
								"Inverse"
							};
							static char antiaim_pitch_types[][PPGUI_MAX_STRING_LENGTH] =
							{
								"Viewangle",
								"Emotion",
								"Up",
								"Random"
							};

							static int selected_antiaim_tab = 0;
							switch (PPGUI_Tab(4, antiaim_tabs, selected_antiaim_tab, false))
							{
							case 0: /// Standing
							{
								PPGUI_Separator("", false);
								PPGUI_Checkbox("At target##standing at target", SETTINGS::ragebot_configs.antiaim_standing_at_target_enabled);

								PPGUI_SameLine();
								PPGUI_Combobox("Pitch##standing pitch", 4, antiaim_pitch_types, SETTINGS::ragebot_configs.antiaim_standing_pitch);

								/* REAL */
								PPGUI_Separator("Real");
								PPGUI_Combobox("Yaw##standing real yaw", 5, antiaim_real_types, SETTINGS::ragebot_configs.antiaim_standing_real_yaw);

								PPGUI_Slider("Offset##standing real offset", "°", -179.f, 179.f, SETTINGS::ragebot_configs.antiaim_standing_real_offset);
								PPGUI_Slider("LBY delta##standing lby delta", "°", -179.f, 179.f, SETTINGS::ragebot_configs.antiaim_standing_real_lby_delta);
								PPGUI_Slider("Jitter range##standing real jitter range", "°", 0.f, 360.f, SETTINGS::ragebot_configs.antiaim_standing_real_jrange);

								/// spin
								if (SETTINGS::ragebot_configs.antiaim_standing_real_yaw == 4)
									PPGUI_Slider("Spin speed##standing real spin speed", "", -20.f, 20.f, SETTINGS::ragebot_configs.antiaim_standing_real_spin_speed);
								else
									SETTINGS::ragebot_configs.antiaim_standing_real_spin_speed = 0.f;

								/* FAKE */
								PPGUI_Separator("Fake");
								PPGUI_Combobox("Yaw##standing fake yaw", 6, antiaim_fake_types, SETTINGS::ragebot_configs.antiaim_standing_fake_yaw);

								PPGUI_Slider("Offset##standing fake offset", "°", -179.f, 179.f, SETTINGS::ragebot_configs.antiaim_standing_fake_offset);
								PPGUI_Slider("Jitter range##standing fake jitter range", "°", 0.f, 360.f, SETTINGS::ragebot_configs.antiaim_standing_fake_jrange);

								/// spin
								if (SETTINGS::ragebot_configs.antiaim_standing_fake_yaw == 4)
									PPGUI_Slider("Spin speed##standing real spin speed", "", -20.f, 20.f, SETTINGS::ragebot_configs.antiaim_standing_fake_spin_speed);
								else
									SETTINGS::ragebot_configs.antiaim_standing_fake_spin_speed = 0.f;

								break;
							}
							case 1: /// Moving
							{
								PPGUI_Separator("", false);
								PPGUI_Checkbox("At target##moving at target", SETTINGS::ragebot_configs.antiaim_moving_at_target_enabled);

								PPGUI_SameLine();
								PPGUI_Combobox("Pitch##moving pitch", 4, antiaim_pitch_types, SETTINGS::ragebot_configs.antiaim_moving_pitch);

								/* REAL */
								PPGUI_Separator("Real");
								PPGUI_Combobox("Yaw##moving real yaw", 5, antiaim_real_types, SETTINGS::ragebot_configs.antiaim_moving_real_yaw);

								PPGUI_Slider("Offset##moving real offset", "°", -179.f, 179.f, SETTINGS::ragebot_configs.antiaim_moving_real_offset);
								PPGUI_Slider("Jitter range##moving real jitter range", "°", 0.f, 360.f, SETTINGS::ragebot_configs.antiaim_moving_real_jrange);

								/// spin
								if (SETTINGS::ragebot_configs.antiaim_moving_real_yaw == 4)
									PPGUI_Slider("Spin speed##moving real spin speed", "", -20.f, 20.f, SETTINGS::ragebot_configs.antiaim_moving_real_spin_speed);
								else
									SETTINGS::ragebot_configs.antiaim_moving_real_spin_speed = 0.f;

								/* FAKE */
								PPGUI_Separator("Fake");
								PPGUI_Combobox("Yaw##moving fake yaw", 6, antiaim_fake_types, SETTINGS::ragebot_configs.antiaim_moving_fake_yaw);

								PPGUI_Slider("Offset##moving fake offset", "°", -179.f, 179.f, SETTINGS::ragebot_configs.antiaim_moving_fake_offset);
								PPGUI_Slider("Jitter range##moving fake jitter range", "°", 0.f, 360.f, SETTINGS::ragebot_configs.antiaim_moving_fake_jrange);

								/// spin
								if (SETTINGS::ragebot_configs.antiaim_moving_fake_yaw == 4)
									PPGUI_Slider("Spin speed##moving real spin speed", "", -20.f, 20.f, SETTINGS::ragebot_configs.antiaim_moving_fake_spin_speed);
								else
									SETTINGS::ragebot_configs.antiaim_moving_fake_spin_speed = 0.f;

								break;
							}
							case 2: /// Jumping
							{
								PPGUI_Separator("", false);
								PPGUI_Checkbox("At target##jumping at target", SETTINGS::ragebot_configs.antiaim_jumping_at_target_enabled);

								PPGUI_SameLine();
								PPGUI_Combobox("Pitch##jumping pitch", 4, antiaim_pitch_types, SETTINGS::ragebot_configs.antiaim_jumping_pitch);

								/* REAL */
								PPGUI_Separator("Real");
								PPGUI_Combobox("Yaw##jumping real yaw", 5, antiaim_real_types, SETTINGS::ragebot_configs.antiaim_jumping_real_yaw);

								PPGUI_Slider("Offset##jumping real offset", "°", -179.f, 179.f, SETTINGS::ragebot_configs.antiaim_jumping_real_offset);
								PPGUI_Slider("Jitter range##jumping real jitter range", "°", 0.f, 360.f, SETTINGS::ragebot_configs.antiaim_jumping_real_jrange);

								/// spin
								if (SETTINGS::ragebot_configs.antiaim_jumping_real_yaw == 4)
									PPGUI_Slider("Spin speed##jumping real spin speed", "", -20.f, 20.f, SETTINGS::ragebot_configs.antiaim_jumping_real_spin_speed);
								else
									SETTINGS::ragebot_configs.antiaim_jumping_real_spin_speed = 0.f;

								/* FAKE */
								PPGUI_Separator("Fake");
								PPGUI_Combobox("Yaw##jumping fake yaw", 6, antiaim_fake_types, SETTINGS::ragebot_configs.antiaim_jumping_fake_yaw);

								PPGUI_Slider("Offset##jumping fake offset", "°", -179.f, 179.f, SETTINGS::ragebot_configs.antiaim_jumping_fake_offset);
								PPGUI_Slider("Jitter range##jumping fake jitter range", "°", 0.f, 360.f, SETTINGS::ragebot_configs.antiaim_jumping_fake_jrange);

								/// spin
								if (SETTINGS::ragebot_configs.antiaim_jumping_fake_yaw == 4)
									PPGUI_Slider("Spin speed##jumping real spin speed", "", -20.f, 20.f, SETTINGS::ragebot_configs.antiaim_jumping_fake_spin_speed);
								else
									SETTINGS::ragebot_configs.antiaim_jumping_fake_spin_speed = 0.f;

								break;
							}
							case 3: /// Edging
							{
								static char antiaim_edging_types[][PPGUI_MAX_STRING_LENGTH] =
								{
									"Wall detection",
									"Freestanding",
									"Freestanding + Wall detection"
								};
								PPGUI_Separator("", false);
								PPGUI_Combobox("Edging type", 3, antiaim_edging_types, SETTINGS::ragebot_configs.antiaim_edging_type, false);

								PPGUI_Slider("LBY delta", "°", -179.f, 179.f, SETTINGS::ragebot_configs.antiaim_edging_lby_delta, 1, false);
								PPGUI_Slider("Jitter range", "°", 0.f, 360.f, SETTINGS::ragebot_configs.antiaim_edging_jrange, 1, false);

								PPGUI_Checkbox("Standing", SETTINGS::ragebot_configs.antiaim_edging_standing_enabled, false);

								PPGUI_SameLine();
								PPGUI_Checkbox("Moving", SETTINGS::ragebot_configs.antiaim_edging_moving_enabled, false);

								PPGUI_SameLine();
								PPGUI_Checkbox("In-air", SETTINGS::ragebot_configs.antiaim_edging_jumping_enabled, false);

								break;
							}
							}
						}
					} PPGUI_EndGroupbox();

					break;
				}
				case 1: /// Legitbot
				{
					PPGUI_SetColumn(0);

					PPGUI_BeginGroupbox("", 200, 1, false);
					{
						static bool test_bool_1 = false;
						PPGUI_Checkbox("Test", test_bool_1, false);
					} PPGUI_EndGroupbox();

					break;
				}
				case 2: /// Visuals
				{
					PPGUI_SetColumn(1);

					PPGUI_BeginGroupbox("Other##visuals other", 270);
					{
						PPGUI_Separator("Thirdperson", false);
						{
							PPGUI_Checkbox("Enabled##thirdperson enabled", SETTINGS::main_configs.thirdperson_enabled, false);
							PPGUI_AttachToPreviousWidget();
							PPGUI_Keybind("##thirdperson keybind", SETTINGS::main_configs.thirdperson_keybind, false);

							static char thirdperson_angles[][PPGUI_MAX_STRING_LENGTH] =
							{
								"Real",
								"Fake",
								"LBY"
							};

							if (SETTINGS::main_configs.thirdperson_enabled)
							{
								PPGUI_Slider("Distance##thirdperson distance", "", 50.f, 500.f, SETTINGS::main_configs.thirdperson_distance, 0, false);

								PPGUI_Combobox("Angle##thirdperson angle", 3, thirdperson_angles, SETTINGS::main_configs.thirdperson_angle, false);
								PPGUI_ToolTip("The yaw that is displayed");
							}
							else
							{
								PPGUI_Disable();
								PPGUI_AlphaModulate(0.2f);
								PPGUI_Slider("Distance##thirdperson distance", "", 50.f, 500.f, SETTINGS::main_configs.thirdperson_distance, 0, false);

								PPGUI_Disable();
								PPGUI_AlphaModulate(0.2f);
								PPGUI_Combobox("Angle##thirdperson angle", 3, thirdperson_angles, SETTINGS::main_configs.thirdperson_angle, false);
								PPGUI_ToolTip("The yaw that is displayed");
							}
						}

						PPGUI_Separator("Misc##visuals other misc", false);
						{
							PPGUI_Slider("Fov##visual fov", "", 1.f, 179.f, SETTINGS::main_configs.visual_fov, 0, false);
							PPGUI_Slider("Scoped transparency", "%", 0.f, 100.f, SETTINGS::main_configs.transparency_when_scoped, 1, false);
							PPGUI_ToolTip("How transparent you are while scoped in");

							PPGUI_Checkbox("Autowall crosshair", SETTINGS::main_configs.autowall_crosshair_enabled, false);
							PPGUI_ToolTip("Changes crosshair color based on wall penetration");
							PPGUI_SameLine();
							PPGUI_Checkbox("Hitmarkers", SETTINGS::main_configs.hitmarkers_enabled, false);

							if (PPGUI_Checkbox("Spread circle", SETTINGS::main_configs.spread_circle_enabled, false))
							{
								PPGUI_AttachToPreviousWidget();
								PPGUI_ColorPicker("##spread circle color", SETTINGS::main_configs.spread_circle_color.RGBA[0],
									SETTINGS::main_configs.spread_circle_color.RGBA[1],
									SETTINGS::main_configs.spread_circle_color.RGBA[2],
									SETTINGS::main_configs.spread_circle_color.RGBA[3], false);
							}

							if (PPGUI_Checkbox("Bullet tracers", SETTINGS::main_configs.bullet_tracers_enabled, false))
							{
								PPGUI_AttachToPreviousWidget();
								PPGUI_ColorPicker("##bullet tracers color", SETTINGS::main_configs.bullet_tracers_color.RGBA[0],
									SETTINGS::main_configs.bullet_tracers_color.RGBA[1],
									SETTINGS::main_configs.bullet_tracers_color.RGBA[2],
									SETTINGS::main_configs.bullet_tracers_color.RGBA[3], false);
							}

							if (PPGUI_Checkbox("Snaplines", SETTINGS::main_configs.snaplines_enabled, false))
							{
								PPGUI_AttachToPreviousWidget();
								PPGUI_ColorPicker("##snaplines color", SETTINGS::main_configs.snaplines_color.RGBA[0],
									SETTINGS::main_configs.snaplines_color.RGBA[1],
									SETTINGS::main_configs.snaplines_color.RGBA[2],
									SETTINGS::main_configs.snaplines_color.RGBA[3], false);
							}

							if (PPGUI_Checkbox("Grenade tracers", SETTINGS::main_configs.grenade_tracers_enabled, false))
							{
								PPGUI_AttachToPreviousWidget();
								PPGUI_ColorPicker("##grenade tracers color", SETTINGS::main_configs.grenade_tracers_color.RGBA[0],
									SETTINGS::main_configs.grenade_tracers_color.RGBA[1],
									SETTINGS::main_configs.grenade_tracers_color.RGBA[2],
									SETTINGS::main_configs.grenade_tracers_color.RGBA[3], false);
							}

							if (PPGUI_Checkbox("No scope overlay", SETTINGS::main_configs.no_scope_overlay_enabled, false))
							{
								PPGUI_AttachToPreviousWidget();
								PPGUI_ColorPicker("##no scope overlay color", SETTINGS::main_configs.no_scope_overlay_color.RGBA[0],
									SETTINGS::main_configs.no_scope_overlay_color.RGBA[1],
									SETTINGS::main_configs.no_scope_overlay_color.RGBA[2],
									SETTINGS::main_configs.no_scope_overlay_color.RGBA[3], false);
							}
						}
					} PPGUI_EndGroupbox();

					PPGUI_BeginGroupbox("World textures##visuals world textures", 60);
					{
						PPGUI_Combobox("Skybox##skybox combobox", FEATURES::VISUALS::world_textures.NUM_SKYBOXES, FEATURES::VISUALS::world_textures.skybox_list, SETTINGS::main_configs.world_textures_skybox, false);

						PPGUI_AttachToPreviousWidget();
						PPGUI_ColorPicker("##skybox color modulation", SETTINGS::main_configs.world_textures_skybox_color.RGBA[0],
							SETTINGS::main_configs.world_textures_skybox_color.RGBA[1],
							SETTINGS::main_configs.world_textures_skybox_color.RGBA[2],
							SETTINGS::main_configs.world_textures_skybox_color.RGBA[3], false);

						PPGUI_ColorPicker("World##world color modulation", SETTINGS::main_configs.world_textures_world_color.RGBA[0],
							SETTINGS::main_configs.world_textures_world_color.RGBA[1],
							SETTINGS::main_configs.world_textures_world_color.RGBA[2],
							SETTINGS::main_configs.world_textures_world_color.RGBA[3], false);

						PPGUI_SameLine();
						PPGUI_ColorPicker("Props##static prop color modulation", SETTINGS::main_configs.world_textures_static_props_color.RGBA[0],
							SETTINGS::main_configs.world_textures_static_props_color.RGBA[1],
							SETTINGS::main_configs.world_textures_static_props_color.RGBA[2],
							SETTINGS::main_configs.world_textures_static_props_color.RGBA[3], false);
					} PPGUI_EndGroupbox();

					PPGUI_BeginGroupbox("Backtrack visualization", 80);
					{
						PPGUI_Checkbox("Skeleton##backtrack skeleton enabled", SETTINGS::main_configs.bt_vis_skeleton_enabled, false);
						PPGUI_AttachToPreviousWidget();
						PPGUI_ColorPicker("##backtrack skeleton color", SETTINGS::main_configs.bt_vis_skeleton_color.RGBA[0], 
							SETTINGS::main_configs.bt_vis_skeleton_color.RGBA[1],
							SETTINGS::main_configs.bt_vis_skeleton_color.RGBA[2], 
							SETTINGS::main_configs.bt_vis_skeleton_color.RGBA[3], false);

						PPGUI_Checkbox("Chams##backtrack chams enabled", SETTINGS::main_configs.bt_vis_chams_enabled, false);
						PPGUI_AttachToPreviousWidget();
						PPGUI_ColorPicker("##backtrack chams color", SETTINGS::main_configs.bt_vis_chams_color.RGBA[0],
							SETTINGS::main_configs.bt_vis_chams_color.RGBA[1],
							SETTINGS::main_configs.bt_vis_chams_color.RGBA[2],
							SETTINGS::main_configs.bt_vis_chams_color.RGBA[3], false);

						PPGUI_Checkbox("Dot##backtrack dot enabled", SETTINGS::main_configs.bt_vis_dot_enabled, false);
						PPGUI_AttachToPreviousWidget();
						PPGUI_ColorPicker("##backtrack dot color", SETTINGS::main_configs.bt_vis_dot_color.RGBA[0],
							SETTINGS::main_configs.bt_vis_dot_color.RGBA[1],
							SETTINGS::main_configs.bt_vis_dot_color.RGBA[2],
							SETTINGS::main_configs.bt_vis_dot_color.RGBA[3], false);
					} PPGUI_EndGroupbox();


					PPGUI_SetColumn(2); 

					PPGUI_BeginGroupbox("ESP##visuals esp", 210);
					{
						/* ENEMY */
						PPGUI_Separator("Enemy##esp enemy", false);
						
						/// box
						if (PPGUI_Checkbox("Box##enemy box esp enabled", SETTINGS::main_configs.esp_enemy_box_enabled, false))
						{
							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##enemy box esp color", SETTINGS::main_configs.esp_enemy_box_color.RGBA[0], 
								SETTINGS::main_configs.esp_enemy_box_color.RGBA[1], 
								SETTINGS::main_configs.esp_enemy_box_color.RGBA[2], 
								SETTINGS::main_configs.esp_enemy_box_color.RGBA[3], false);
						}

						/// skeleton
						if (PPGUI_Checkbox("Skeleton##enemy skeleton esp enabled", SETTINGS::main_configs.esp_enemy_skeleton_enabled, false))
						{
							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##enemy skeleton esp color", SETTINGS::main_configs.esp_enemy_skeleton_color.RGBA[0],
								SETTINGS::main_configs.esp_enemy_skeleton_color.RGBA[1],
								SETTINGS::main_configs.esp_enemy_skeleton_color.RGBA[2],
								SETTINGS::main_configs.esp_enemy_skeleton_color.RGBA[3], false);
						}

						/// name esp
						if (PPGUI_Checkbox("Name##enemy esp name", SETTINGS::main_configs.esp_enemy_name_enabled, false))
						{
							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##enemy name esp color", SETTINGS::main_configs.esp_enemy_text_color.RGBA[0],
								SETTINGS::main_configs.esp_enemy_text_color.RGBA[1],
								SETTINGS::main_configs.esp_enemy_text_color.RGBA[2],
								SETTINGS::main_configs.esp_enemy_text_color.RGBA[3], false);
						}

						PPGUI_Checkbox("Weapon##enemy esp weapon", SETTINGS::main_configs.esp_enemy_weapon_enabled, false);
						PPGUI_SameLine();
						PPGUI_Checkbox("Health##enemy esp health", SETTINGS::main_configs.esp_enemy_health_enabled, false);
						PPGUI_SameLine();
						PPGUI_Checkbox("Resolver info##enemy esp resolver info", SETTINGS::main_configs.esp_enemy_resolve_enabled, false);
						PPGUI_ToolTip("Displays what the player is currently being resolved at");

						/* TEAM */
						PPGUI_Separator("Team##esp team", false);

						/// box
						if (PPGUI_Checkbox("Box##team box esp enabled", SETTINGS::main_configs.esp_team_box_enabled, false))
						{
							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##team box esp color", SETTINGS::main_configs.esp_team_box_color.RGBA[0],
								SETTINGS::main_configs.esp_team_box_color.RGBA[1],
								SETTINGS::main_configs.esp_team_box_color.RGBA[2],
								SETTINGS::main_configs.esp_team_box_color.RGBA[3], false);
						}

						/// skeleton
						if (PPGUI_Checkbox("Skeleton##team skeleton esp enabled", SETTINGS::main_configs.esp_team_skeleton_enabled, false))
						{
							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##team skeleton esp color", SETTINGS::main_configs.esp_team_skeleton_color.RGBA[0],
								SETTINGS::main_configs.esp_team_skeleton_color.RGBA[1],
								SETTINGS::main_configs.esp_team_skeleton_color.RGBA[2],
								SETTINGS::main_configs.esp_team_skeleton_color.RGBA[3], false);
						}

						/// name esp
						if (PPGUI_Checkbox("Name##team esp name", SETTINGS::main_configs.esp_team_name_enabled, false))
						{
							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##team name esp color", SETTINGS::main_configs.esp_team_text_color.RGBA[0],
								SETTINGS::main_configs.esp_team_text_color.RGBA[1],
								SETTINGS::main_configs.esp_team_text_color.RGBA[2],
								SETTINGS::main_configs.esp_team_text_color.RGBA[3], false);
						}

						PPGUI_Checkbox("Weapon##team esp weapon", SETTINGS::main_configs.esp_team_weapon_enabled, false);
						PPGUI_SameLine();
						PPGUI_Checkbox("Health##team esp health", SETTINGS::main_configs.esp_team_health_enabled, false);
					} PPGUI_EndGroupbox();

					PPGUI_BeginGroupbox("Player textures", 320);
					{
						static char glow_types[][PPGUI_MAX_STRING_LENGTH] =
						{
							"Outline",
							"Model",
							"Inline",
							"Pulse"
						};

						/// localplayer
						PPGUI_Separator("Localplayer##localplayer player textures seperator", false);
						PPGUI_Checkbox("Draw player##draw localplayer", SETTINGS::main_configs.draw_local_player, false);

						/// chams
						if (PPGUI_Checkbox("Draw chams##draw chams localplayer", SETTINGS::main_configs.chams_local_enabled, false))
						{
							PPGUI_SameLine();
							PPGUI_Checkbox("Wireframe##chams localplayer wireframe enabled", SETTINGS::main_configs.chams_local_wireframe_enabled, false);

							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##localplayer chams color", SETTINGS::main_configs.chams_local_color.RGBA[0],
								SETTINGS::main_configs.chams_local_color.RGBA[1], SETTINGS::main_configs.chams_local_color.RGBA[2],
								SETTINGS::main_configs.chams_local_color.RGBA[3], false);
						}
						else
						{
							PPGUI_Disable();
							PPGUI_AlphaModulate(0.2f);
							PPGUI_SameLine();
							PPGUI_Checkbox("Wireframe##chams localplayer wireframe enabled", SETTINGS::main_configs.chams_local_wireframe_enabled, false);
						}
						/// glow
						if (PPGUI_Checkbox("Draw glow##draw glow localplayer", SETTINGS::main_configs.glow_local_enabled, false))
						{
							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##localplayer glow color", SETTINGS::main_configs.glow_local_color.RGBA[0],
								SETTINGS::main_configs.glow_local_color.RGBA[1], SETTINGS::main_configs.glow_local_color.RGBA[2],
								SETTINGS::main_configs.glow_local_color.RGBA[3], false);

							if (PPGUI_Combobox("##localplayer glow style", 4, glow_types, SETTINGS::main_configs.glow_local_style, false) != 0)
							{
								PPGUI_Disable();
								PPGUI_AlphaModulate(0.2f);
							}

							PPGUI_AttachToPreviousWidget();
							PPGUI_Checkbox("Fullbloom##localplayer glow fullbloom", SETTINGS::main_configs.glow_local_fullbloom_enabled, false);
						}
						else
						{
							PPGUI_Disable();
							PPGUI_AlphaModulate(0.2f);
							PPGUI_Combobox("##localplayer glow style", 4, glow_types, SETTINGS::main_configs.glow_local_style, false);

							PPGUI_Disable();
							PPGUI_AlphaModulate(0.2f);
							PPGUI_AttachToPreviousWidget();
							PPGUI_Checkbox("Fullbloom##localplayer glow fullbloom", SETTINGS::main_configs.glow_local_fullbloom_enabled, false);
						}

						/// enemies
						PPGUI_Separator("Enemy##enemy player textures seperator", false);
						PPGUI_Checkbox("Draw player##draw enemies", SETTINGS::main_configs.draw_enemy_player, false);

						/// chams
						if (PPGUI_Checkbox("Draw chams##draw chams enemy", SETTINGS::main_configs.chams_enemy_enabled, false))
						{
							PPGUI_SameLine();
							PPGUI_Checkbox("Wireframe##chams enemy wireframe enabled", SETTINGS::main_configs.chams_enemy_wireframe_enabled, false);

							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##enemy occluded chams color", SETTINGS::main_configs.chams_enemy_occluded_color.RGBA[0],
								SETTINGS::main_configs.chams_enemy_occluded_color.RGBA[1], SETTINGS::main_configs.chams_enemy_occluded_color.RGBA[2],
								SETTINGS::main_configs.chams_enemy_occluded_color.RGBA[3], false);
							PPGUI_ToolTip("Chams color while behind walls##enemy");

							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##enemy unoccluded chams color", SETTINGS::main_configs.chams_enemy_unoccluded_color.RGBA[0],
								SETTINGS::main_configs.chams_enemy_unoccluded_color.RGBA[1], SETTINGS::main_configs.chams_enemy_unoccluded_color.RGBA[2],
								SETTINGS::main_configs.chams_enemy_unoccluded_color.RGBA[3], false);
							PPGUI_ToolTip("Chams color while visible##enemy");
						}
						else
						{
							PPGUI_Disable();
							PPGUI_AlphaModulate(0.2f);
							PPGUI_SameLine();
							PPGUI_Checkbox("Wireframe##chams enemy wireframe enabled", SETTINGS::main_configs.chams_enemy_wireframe_enabled, false);
						}
						/// glow
						if (PPGUI_Checkbox("Draw glow##draw glow enemy", SETTINGS::main_configs.glow_enemy_enabled, false))
						{
							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##enemy glow color", SETTINGS::main_configs.glow_enemy_color.RGBA[0],
								SETTINGS::main_configs.glow_enemy_color.RGBA[1], SETTINGS::main_configs.glow_enemy_color.RGBA[2],
								SETTINGS::main_configs.glow_enemy_color.RGBA[3], false);

							if (PPGUI_Combobox("##enemy glow style", 4, glow_types, SETTINGS::main_configs.glow_enemy_style, false) != 0)
							{
								PPGUI_Disable();
								PPGUI_AlphaModulate(0.2f);
							}
							
							PPGUI_AttachToPreviousWidget();
							PPGUI_Checkbox("Fullbloom##enemy glow fullbloom", SETTINGS::main_configs.glow_enemy_fullbloom_enabled, false);
						}
						else
						{
							PPGUI_Disable();
							PPGUI_AlphaModulate(0.2f);
							PPGUI_Combobox("##enemy glow style", 4, glow_types, SETTINGS::main_configs.glow_enemy_style, false);

							PPGUI_Disable();
							PPGUI_AlphaModulate(0.2f);
							PPGUI_AttachToPreviousWidget();
							PPGUI_Checkbox("Fullbloom##enemy glow fullbloom", SETTINGS::main_configs.glow_enemy_fullbloom_enabled, false);
						}

						/// teammates
						PPGUI_Separator("Team##team player textures seperator", false);
						PPGUI_Checkbox("Draw player##draw team", SETTINGS::main_configs.draw_team_player, false);

						/// chams
						if (PPGUI_Checkbox("Draw chams##draw chams team", SETTINGS::main_configs.chams_team_enabled, false))
						{
							PPGUI_SameLine();
							PPGUI_Checkbox("Wireframe##chams team wireframe enabled", SETTINGS::main_configs.chams_team_wireframe_enabled, false);

							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##team occluded chams color", SETTINGS::main_configs.chams_team_occluded_color.RGBA[0],
								SETTINGS::main_configs.chams_team_occluded_color.RGBA[1], SETTINGS::main_configs.chams_team_occluded_color.RGBA[2],
								SETTINGS::main_configs.chams_team_occluded_color.RGBA[3], false);
							PPGUI_ToolTip("Chams color while behind walls##team");

							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##team unoccluded chams color", SETTINGS::main_configs.chams_team_unoccluded_color.RGBA[0],
								SETTINGS::main_configs.chams_team_unoccluded_color.RGBA[1], SETTINGS::main_configs.chams_team_unoccluded_color.RGBA[2],
								SETTINGS::main_configs.chams_team_unoccluded_color.RGBA[3], false);
							PPGUI_ToolTip("Chams color while visible##team");
						}
						else
						{
							PPGUI_Disable();
							PPGUI_AlphaModulate(0.2f);
							PPGUI_SameLine();
							PPGUI_Checkbox("Wireframe##chams team wireframe enabled", SETTINGS::main_configs.chams_team_wireframe_enabled, false);
						}
						/// glow
						if (PPGUI_Checkbox("Draw glow##draw glow team", SETTINGS::main_configs.glow_team_enabled, false))
						{
							PPGUI_AttachToPreviousWidget();
							PPGUI_ColorPicker("##team glow color", SETTINGS::main_configs.glow_team_color.RGBA[0],
								SETTINGS::main_configs.glow_team_color.RGBA[1], SETTINGS::main_configs.glow_team_color.RGBA[2],
								SETTINGS::main_configs.glow_team_color.RGBA[3], false);

							if (PPGUI_Combobox("##team glow style", 4, glow_types, SETTINGS::main_configs.glow_team_style, false) == 0)
							{
								PPGUI_Disable();
								PPGUI_AlphaModulate(0.2f);
							}

							PPGUI_AttachToPreviousWidget();
							PPGUI_Checkbox("Fullbloom##team glow fullbloom", SETTINGS::main_configs.glow_team_fullbloom_enabled, false);
						}
						else
						{
							PPGUI_Disable();
							PPGUI_AlphaModulate(0.2f);
							PPGUI_Combobox("##team glow style", 4, glow_types, SETTINGS::main_configs.glow_team_style, false);

							PPGUI_Disable();
							PPGUI_AlphaModulate(0.2f);
							PPGUI_AttachToPreviousWidget();
							PPGUI_Checkbox("Fullbloom##team glow fullbloom", SETTINGS::main_configs.glow_team_fullbloom_enabled, false);
						}

					} PPGUI_EndGroupbox();

					break;
				}
				case 3: /// Misc
				{
					PPGUI_SetColumn(1);

					PPGUI_BeginGroupbox("Configs", 200);
					{
						PPGUI_Separator("Ragebot", false);
						{
							static char ragebot_configs[20][PPGUI_MAX_STRING_LENGTH];
							static int selected_ragebot_config = 0;

							/// No configs found
							if (const int ragebot_configs_num = SETTINGS::Configuration::GetConfigs(ragebot_configs, ".rcfg"); ragebot_configs_num <= 0)
							{
								/// Creating a new config
								static char new_config_name[PPGUI_MAX_STRING_LENGTH];
								PPGUI_TextInput("##ragebot new config name", new_config_name, false);

								if (PPGUI_SameLine(); PPGUI_Button("New##ragebot", false))
								{
									/// clear the string after they create the config
									if (SETTINGS::Configuration::CreateConfig(new_config_name, ".rcfg"))
										new_config_name[0] = '\0';
								}
							}
							else /// Found atleast one config
							{
								/// Creating a new config
								static char new_config_name[PPGUI_MAX_STRING_LENGTH];
								PPGUI_TextInput("##rage config", new_config_name, false);

								if (PPGUI_SameLine(); PPGUI_Button("New##ragebot", false))
								{
									/// clear the string after they create the config
									if (SETTINGS::Configuration::CreateConfig(new_config_name, ".rcfg"))
										new_config_name[0] = '\0';
								}

								PPGUI_Combobox("##ragebot config", ragebot_configs_num, ragebot_configs, selected_ragebot_config, false);
								if (PPGUI_SameLine(); PPGUI_Button("Delete##ragebot", false))
									SETTINGS::ragebot_configs.DeleteConfig(ragebot_configs[selected_ragebot_config], ".rcfg");

								/// Loading, saving
								if (PPGUI_Button("Save##ragebot", false))
									SETTINGS::ragebot_configs.Save(ragebot_configs[selected_ragebot_config], ".rcfg", sizeof(SETTINGS::RagebotConfig));
								if (PPGUI_SameLine(); PPGUI_Button("Load##ragebot", false))
									SETTINGS::ragebot_configs.Load(ragebot_configs[selected_ragebot_config], ".rcfg", sizeof(SETTINGS::RagebotConfig));
							}
						}
						PPGUI_Separator("Visuals / Misc", false);
						{
							static char main_configs[20][PPGUI_MAX_STRING_LENGTH];
							static int selected_main_config = 0;

							/// No configs found
							if (const int main_configs_num = SETTINGS::Configuration::GetConfigs(main_configs, ".vmcfg"); main_configs_num <= 0)
							{
								/// Creating a new config
								static char new_config_name[PPGUI_MAX_STRING_LENGTH];
								PPGUI_TextInput("##visuals/misc new config name", new_config_name, false);

								if (PPGUI_SameLine(); PPGUI_Button("New##main", false))
								{
									/// clear the string after they create the config
									if (SETTINGS::Configuration::CreateConfig(new_config_name, ".vmcfg"))
										new_config_name[0] = '\0';
								}
							}
							else /// Found atleast one config
							{
								/// Creating a new config
								static char new_config_name[PPGUI_MAX_STRING_LENGTH];
								PPGUI_TextInput("##main config", new_config_name, false);

								if (PPGUI_SameLine(); PPGUI_Button("New##main", false))
								{
									/// clear the string after they create the config
									if (SETTINGS::Configuration::CreateConfig(new_config_name, ".vmcfg"))
										new_config_name[0] = '\0';
								}

								PPGUI_Combobox("##main config", main_configs_num, main_configs, selected_main_config, false);

								if (PPGUI_SameLine(); PPGUI_Button("Delete##main", false))
									SETTINGS::main_configs.DeleteConfig(main_configs[selected_main_config], ".vmcfg");

								/// Loading, saving
								if (PPGUI_Button("Save##main", false))
									SETTINGS::main_configs.Save(main_configs[selected_main_config], ".vmcfg", sizeof(SETTINGS::MainConfig));
								if (PPGUI_SameLine(); PPGUI_Button("Load##main", false))
									SETTINGS::main_configs.Load(main_configs[selected_main_config], ".vmcfg", sizeof(SETTINGS::MainConfig));
							}
						}
					} PPGUI_EndGroupbox();

					PPGUI_SetColumn(2);

					PPGUI_BeginGroupbox("Misc##misc", 250);
					{
						if (PPGUI_Button("Unload cheat", false))
						{
							Uninject();
						}
						PPGUI_Separator("##unload cheat seperator", false);

						PPGUI_Checkbox("Anti-untrusted", SETTINGS::main_configs.anti_untrusted_enabled, false);
						PPGUI_SameLine();
						PPGUI_Checkbox("No flash", SETTINGS::main_configs.no_flash_enabled, false);
						PPGUI_SameLine();
						PPGUI_Checkbox("No smoke", SETTINGS::main_configs.no_smoke_enabled, false);
						PPGUI_SameLine();
						PPGUI_Checkbox("Radar", SETTINGS::main_configs.radar_enabled, false);

						PPGUI_Checkbox("Bhop", SETTINGS::main_configs.bhop_enabled, false);
						PPGUI_SameLine();
						PPGUI_Checkbox("Auto-strafer", SETTINGS::main_configs.autostrafer_enabled, false);
						PPGUI_SameLine();
						PPGUI_Checkbox("Auto-revolver", SETTINGS::main_configs.auto_revolver_enabled, false);

						PPGUI_Keybind("Circle strafer", SETTINGS::main_configs.circle_strafer_keybind, false);
						PPGUI_Keybind("Override", SETTINGS::main_configs.override_keybind, false);

						/// Clan tag changer customization
						PPGUI_Separator("Clan tag", false);
						static char clan_tag_changer_styles[][PPGUI_MAX_STRING_LENGTH] =
						{
							"Static",
							"Marquee",
							"Shift"
						};

						PPGUI_Checkbox("Enabled##clan tag changer enabled", SETTINGS::main_configs.clantag_changer_enabled, false);
						{
							PPGUI_TextInput("Name##clan tag changer name", SETTINGS::main_configs.clantag_changer_text, false);
							if (PPGUI_Combobox("Style##clan tag changer style", 3, clan_tag_changer_styles, SETTINGS::main_configs.clantag_changer_style, false) != 0)
								PPGUI_Slider("Speed", "", SETTINGS::main_configs.clantag_changer_style == 2 ? 0.f : -1.f, 1.f, SETTINGS::main_configs.clantag_changer_speed, 2, false);
						}
					} PPGUI_EndGroupbox();

					PPGUI_BeginGroupbox("Fake-lag / Fake-walk", 160);
					{
						PPGUI_Separator("Fake-walk", false);

						PPGUI_Keybind("Key##fake-walk key", SETTINGS::main_configs.fakewalk_keybind, false);
						PPGUI_Slider("Speed##fake-walk speed", "", 0.f, 1.f, SETTINGS::main_configs.fakewalk_speed, 2, false);
						PPGUI_Checkbox("On peek##fakewalk on peek", SETTINGS::main_configs.fakewalk_when_peek, false);

						PPGUI_Separator("Fake-lag", false);

						PPGUI_Checkbox("Adaptive##adaptive fakelag enabled", SETTINGS::main_configs.fakelag_adaptive_enabled, false);
						PPGUI_ToolTip("Ideal fakelag amount when jumping");

						PPGUI_SameLine();
						PPGUI_Checkbox("On peek##fakelag on peek", SETTINGS::main_configs.fakelag_on_peek, false);
						PPGUI_ToolTip("Max fakelag when peeking");

						PPGUI_Slider("Moving##fakelag moving", "", 1.f, 14.f, SETTINGS::main_configs.fakelag_moving, 0, false);

						if (SETTINGS::main_configs.fakelag_adaptive_enabled)
						{
							PPGUI_Disable(); 
							PPGUI_AlphaModulate(0.2f);
						}

						PPGUI_Slider("In-air##fakelag in air", "", 1.f, 14.f, SETTINGS::main_configs.fakelag_jumping, 0, false);
					} PPGUI_EndGroupbox();

					break;
				}
				}
			} PPGUI_EndWindow();

			/// End
			PPGUI_EndFrame();
		}
	}
}