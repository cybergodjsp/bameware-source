#pragma once

#include "SkinChanger.h"

#include "../PPGUI Framework/PPGUI.h"

namespace SETTINGS
{
	class Configuration
	{
	public:
		bool Save(const char* file_name, const size_t size);
		bool Load(const char* file_name, const size_t size);

		bool Save(const char* file_name, const char* extension, const size_t size)
		{
			char name[MAX_PATH];
			sprintf(name, "%s%s", file_name, extension);

			return Save(name, size);
		}

		bool Load(const char* file_name, const char* extension, const size_t size)
		{
			char name[MAX_PATH];
			sprintf(name, "%s%s", file_name, extension);

			return Load(name, size);
		}

		static int GetConfigs(char (*configs)[PPGUI::PPGUI_MAX_STRING_LENGTH], const char* extension);
		static bool CreateConfig(const char* name, const char* extension);
		static bool DeleteConfig(const char* name, const char* extension);
		static bool RenameConfig(const char* current_name, const char* new_name, const char* extension);
	};

	class SkinConfig : public Configuration
	{
	public:
		SkinConfig();

	public:
		int weapon_skin[FEATURES::MISC::skin_changer.NUM_WEAPONS];
		int weapon_quality[FEATURES::MISC::skin_changer.NUM_WEAPONS];
		float weapon_wear[FEATURES::MISC::skin_changer.NUM_WEAPONS];
		float weapon_seed[FEATURES::MISC::skin_changer.NUM_WEAPONS];
		bool weapon_stat_trak_enabled[FEATURES::MISC::skin_changer.NUM_WEAPONS];
		float weapon_stat_trak_kills[FEATURES::MISC::skin_changer.NUM_WEAPONS];
		char weapon_custom_name[FEATURES::MISC::skin_changer.NUM_WEAPONS][PPGUI::PPGUI_MAX_STRING_LENGTH];
	};

	extern SkinConfig skin_configs;

	class RagebotConfig : public Configuration
	{
	public:
		/* Antiaims */
		bool antiaim_enabled = false;

		/* Standing */
		bool antiaim_standing_at_target_enabled = false;
		int antiaim_standing_pitch = 0;
		/// Real
		int antiaim_standing_real_yaw = 0;
		float antiaim_standing_real_offset = 0.f;
		float antiaim_standing_real_lby_delta = 0.f;
		float antiaim_standing_real_jrange = 0.f;
		float antiaim_standing_real_spin_speed = 0.f;
		/// Fake
		int antiaim_standing_fake_yaw = 0;
		float antiaim_standing_fake_offset = 0.f;
		float antiaim_standing_fake_jrange = 0.f;
		float antiaim_standing_fake_spin_speed = 0.f;

		/* Moving */
		bool antiaim_moving_at_target_enabled = false;
		int antiaim_moving_pitch = 0;
		/// Real
		int antiaim_moving_real_yaw = 0;
		float antiaim_moving_real_offset = 0.f;
		float antiaim_moving_real_jrange = 0.f;
		float antiaim_moving_real_spin_speed = 0.f;
		/// Fake
		int antiaim_moving_fake_yaw = 0;
		float antiaim_moving_fake_offset = 0.f;
		float antiaim_moving_fake_jrange = 0.f;
		float antiaim_moving_fake_spin_speed = 0.f;

		/* Jumping */
		bool antiaim_jumping_at_target_enabled = false;
		int antiaim_jumping_pitch = 0;
		/// Real
		int antiaim_jumping_real_yaw = 0;
		float antiaim_jumping_real_offset = 0.f;
		float antiaim_jumping_real_jrange = 0.f;
		float antiaim_jumping_real_spin_speed = 0.f;
		/// Fake
		int antiaim_jumping_fake_yaw = 0;
		float antiaim_jumping_fake_offset = 0.f;
		float antiaim_jumping_fake_jrange = 0.f;
		float antiaim_jumping_fake_spin_speed = 0.f;

		/* Edging */
		int antiaim_edging_type = 0;
		float antiaim_edging_lby_delta = 0.f;
		float antiaim_edging_jrange = 0.f;
		bool antiaim_edging_standing_enabled = false;
		bool antiaim_edging_moving_enabled = false;
		bool antiaim_edging_jumping_enabled = false;

		/* AIMBOT */
		bool aimbot_enabled = false;
		bool aimbot_prefer_baim_enabled = false;
		bool aimbot_baim_if_unplugged = false;
		bool aimbot_autostop_enabled = false;

		float aimbot_pointscale = 0.f;
		float aimbot_minimum_damage = 0.f;
		float aimbot_minimum_autowall_damage = 0.f;
		float aimbot_delay_shot = 0.f;

		int aimbot_accuracy_type = 0;
		float aimbot_accuracy = 0.f;

		bool aimbot_selected_multipoint_hitgroups[4];
		bool aimbot_selected_hitgroups[5];
		float aimbot_hitscan_pointscale = 0.f;
		float aimbot_bodyaim_if_x_damage = 100.f;
	};

	extern RagebotConfig ragebot_configs;

	class MainConfig : public Configuration
	{
	public:
		/// thirdperson
		bool thirdperson_enabled = false;
		float thirdperson_distance = 100.f;
		int thirdperson_angle = 0;
		int thirdperson_keybind = -1;

		/// misc shit
		bool anti_untrusted_enabled = false;
		bool bhop_enabled = false;
		bool autostrafer_enabled = false;
		bool no_flash_enabled = false;
		bool no_smoke_enabled = false;
		bool radar_enabled = false;
		bool auto_revolver_enabled = false;
		int circle_strafer_keybind = -1;

		/// clantag changer
		bool clantag_changer_enabled = false;
		int clantag_changer_style = 0;
		float clantag_changer_speed = 0.f;
		char clantag_changer_text[PPGUI::PPGUI_MAX_STRING_LENGTH];

		/// fakewalk
		int fakewalk_keybind = -1;
		float fakewalk_speed = 1.f;
		bool fakewalk_when_peek = false;

		/// fakelag
		bool fakelag_adaptive_enabled = false;
		bool fakelag_on_peek = false;
		float fakelag_moving = 1.f;
		float fakelag_jumping = 1.f;

		/// misc visuals
		float visual_fov = 90.f;
		float transparency_when_scoped = 100.f;
		bool autowall_crosshair_enabled = false;
		bool hitmarkers_enabled = false;
		bool spread_circle_enabled = false;
		CColor spread_circle_color = WHITE;
		bool bullet_tracers_enabled = false;
		CColor bullet_tracers_color = WHITE;
		bool snaplines_enabled = false;
		CColor snaplines_color = WHITE;
		bool grenade_tracers_enabled = false;
		CColor grenade_tracers_color = WHITE;
		bool no_scope_overlay_enabled = false;
		CColor no_scope_overlay_color = WHITE;

		/// esp shit
		bool esp_enemy_box_enabled = false;
		bool esp_enemy_resolve_enabled = false;
		bool esp_enemy_weapon_enabled = false;
		bool esp_enemy_health_enabled = false;
		bool esp_enemy_name_enabled = false;
		bool esp_enemy_skeleton_enabled = false;
		CColor esp_enemy_box_color = WHITE;
		CColor esp_enemy_text_color = WHITE;
		CColor esp_enemy_skeleton_color = WHITE;

		bool esp_team_box_enabled = false;
		bool esp_team_weapon_enabled = false;
		bool esp_team_health_enabled = false;
		bool esp_team_name_enabled = false;
		bool esp_team_skeleton_enabled = false;
		CColor esp_team_box_color = WHITE;
		CColor esp_team_text_color = WHITE;
		CColor esp_team_skeleton_color = WHITE;

		/* PLAYER TEXTURES */
		bool draw_local_player = true;
		bool draw_enemy_player = true;
		bool draw_team_player = true;

		/// chams
		bool chams_local_enabled = false;
		bool chams_local_wireframe_enabled = false;
		CColor chams_local_color = WHITE;

		bool chams_enemy_enabled = false;
		bool chams_enemy_wireframe_enabled = false;
		CColor chams_enemy_occluded_color = WHITE;
		CColor chams_enemy_unoccluded_color = WHITE;

		bool chams_team_enabled = false;
		bool chams_team_wireframe_enabled = false;
		CColor chams_team_occluded_color = WHITE;
		CColor chams_team_unoccluded_color = WHITE;

		/// glow
		bool glow_local_enabled = false;
		bool glow_local_fullbloom_enabled = false;
		int glow_local_style = 0;
		CColor glow_local_color = WHITE;

		bool glow_enemy_enabled = false;
		bool glow_enemy_fullbloom_enabled = false;
		int glow_enemy_style = 0;
		CColor glow_enemy_color = WHITE;

		bool glow_team_enabled = false;
		bool glow_team_fullbloom_enabled = false;
		int glow_team_style = 0;
		CColor glow_team_color = WHITE;

		/* WORLD TEXTURES */
		int world_textures_skybox = 0;
		CColor world_textures_skybox_color = WHITE;
		CColor world_textures_world_color = WHITE;
		CColor world_textures_static_props_color = WHITE;

		/* RANDOM SHIT */
		int override_keybind = -1;

		/* BACKTRACK VISUALIZATION */
		bool bt_vis_skeleton_enabled = false;
		bool bt_vis_chams_enabled = false;
		bool bt_vis_dot_enabled = false;
		CColor bt_vis_skeleton_color = WHITE;
		CColor bt_vis_chams_color = WHITE;
		CColor bt_vis_dot_color = WHITE;
	};

	extern MainConfig main_configs;
}
