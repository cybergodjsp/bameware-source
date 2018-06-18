#include "../includes.h"

#include "BoxESP.h"
#include "Visuals.h"

#include "../UTILS/interfaces.h"
#include "../UTILS/render.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/Collideable.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CGlobalVars.h"

#include "../FEATURES/Backtracking.h"
#include "../FEATURES/Resolver.h"
#include "../FEATURES/Configurations.h"

namespace FEATURES
{
	namespace VISUALS
	{
		CBoxESP box_esp;

		void CBoxESP::Do()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			for (int i = 0; i < 64; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetHealth() <= 0)
					continue;

				/// dont draw esp on localplayer retard
				if (entity == local_player)
					continue;

				/// get the esp box and text color
				CColor box_color = SETTINGS::main_configs.esp_enemy_box_color, text_color = SETTINGS::main_configs.esp_enemy_text_color;
				if (entity->GetTeam() == local_player->GetTeam())
				{
					box_color = SETTINGS::main_configs.esp_team_box_color;
					text_color = SETTINGS::main_configs.esp_team_text_color;
				}

				/// for dormant esp
				auto GetDormantAlpha = [entity, i](CColor color) -> int
					{
						float pulse = fabs(sin(GetTickCount() * 0.003f));
						float start_fade_alpha = min(color.RGBA[3], 150.f);
						float alpha;
						if (entity->GetIsDormant())
							alpha = (start_fade_alpha - ((player_time_dormant[i] / FADE_OUT_TIME) * (255 - start_fade_alpha))) * pulse;
						else
							alpha = start_fade_alpha + ((player_time_not_dormant[i] / FADE_IN_TIME) * (255 - start_fade_alpha));

						return UTILS::Clamp<int>(alpha, 0, color.RGBA[3]);
					};

				text_color.RGBA[3] = GetDormantAlpha(text_color);
				box_color.RGBA[3] = GetDormantAlpha(box_color);

				if (entity->GetTeam() == local_player->GetTeam()) /// teammates
					DrawBox(entity, text_color, box_color, SETTINGS::main_configs.esp_team_box_enabled,
					        SETTINGS::main_configs.esp_team_name_enabled, SETTINGS::main_configs.esp_team_weapon_enabled,
					        false, SETTINGS::main_configs.esp_team_health_enabled);
				else /// enemies
					DrawBox(entity, text_color, box_color, SETTINGS::main_configs.esp_enemy_box_enabled,
					        SETTINGS::main_configs.esp_enemy_name_enabled, SETTINGS::main_configs.esp_enemy_weapon_enabled,
					        SETTINGS::main_configs.esp_enemy_resolve_enabled, SETTINGS::main_configs.esp_enemy_health_enabled);
			}
		}

		void CBoxESP::DrawBox(SDK::CBaseEntity* entity, CColor text_color, CColor box_color, bool draw_box, bool draw_name, bool draw_weapon, bool draw_resolver, bool draw_healthbar)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			/// get mins and maxs
			Vector bbmin, bbmax;
			entity->GetRenderBounds(bbmin, bbmax);

			Vector bottom = entity->GetVecOrigin() + Vector(0, 0, bbmin.z - 5);
			Vector top = entity->GetVecOrigin() + Vector(0, 0, bbmax.z + 5);

			Vector final_screens[4];
			Vector screen_bottom, screen_top;
			if (!RENDER::WorldToScreen(bottom, screen_bottom) || !RENDER::WorldToScreen(top, screen_top))
				return;

			const int width = (screen_bottom.y - screen_top.y) * 0.55f;
			final_screens[0] = screen_top - Vector(width * 0.5f, 0, 0);
			final_screens[1] = screen_top + Vector(width * 0.5f, 0, 0);
			final_screens[2] = screen_bottom + Vector(width * 0.5f, 0, 0);
			final_screens[3] = screen_bottom - Vector(width * 0.5f, 0, 0);

			if (draw_box)
			{
				/// draw the center lines
				for (int i = 0; i < 3; i++)
					RENDER::DrawLine(final_screens[i].x, final_screens[i].y, final_screens[i + 1].x, final_screens[i + 1].y, box_color);
				RENDER::DrawLine(final_screens[3].x, final_screens[3].y, final_screens[0].x, final_screens[0].y, box_color);

				final_screens[0] -= Vector(1, 1, 0);
				final_screens[1] -= Vector(-1, 1, 0);
				final_screens[2] -= Vector(-1, -1, 0);
				final_screens[3] -= Vector(1, -1, 0);

				/// outline and inside line color
				CColor line_color(0, 0, 0, box_color.RGBA[3]);

				/// draw the inside lines
				for (int i = 0; i < 3; i++)
					RENDER::DrawLine(final_screens[i].x, final_screens[i].y, final_screens[i + 1].x, final_screens[i + 1].y, line_color);
				RENDER::DrawLine(final_screens[3].x, final_screens[3].y, final_screens[0].x, final_screens[0].y, line_color);

				final_screens[0] += Vector(2, 2, 0);
				final_screens[1] += Vector(-2, 2, 0);
				final_screens[2] += Vector(-2, -2, 0);
				final_screens[3] += Vector(2, -2, 0);

				/// draw the outlines
				for (int i = 0; i < 3; i++)
					RENDER::DrawLine(final_screens[i].x, final_screens[i].y, final_screens[i + 1].x, final_screens[i + 1].y, line_color);
				RENDER::DrawLine(final_screens[3].x, final_screens[3].y, final_screens[0].x, final_screens[0].y, line_color);

				/// reset the screens to their original state
				final_screens[0] = screen_top - Vector(width * 0.5f, 0, 0);
				final_screens[1] = screen_top + Vector(width * 0.5f, 0, 0);
				final_screens[2] = screen_bottom + Vector(width * 0.5f, 0, 0);
				final_screens[3] = screen_bottom - Vector(width * 0.5f, 0, 0);
			}

			/// draw player info like name and weapons etc
			DrawBoxInfo(entity, text_color, final_screens, draw_name, draw_weapon, draw_resolver);

			/// healthbar
			if (draw_healthbar)
				DrawHealthBar(entity, final_screens, box_color.RGBA[3]);
		}

		void CBoxESP::DrawBoxInfo(SDK::CBaseEntity* entity, CColor text_color, Vector* corners, bool draw_name, bool draw_weapon, bool draw_resolver)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			auto weapon = INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex());
			if (!weapon)
				return;

			auto weapon_clientclass = weapon->GetClientClass();
			if (!weapon_clientclass)
				return;

			const float distance = (local_player->GetVecOrigin() - entity->GetVecOrigin()).Length();
			const unsigned int font = FONTS::visuals_esp_font;

			const auto top_right = corners[1];
			const auto top_center = (corners[0] + corners[1]) * 0.5f;
			const auto bottom_center = (corners[2] + corners[3]) * 0.5f;

			/// waiting list for shit to draw
			std::vector<std::pair<std::string, CColor>> text_queue;

			if (draw_resolver && !(entity->GetIsDormant()) && local_player->GetHealth() > 0)
			{
				char resolve_state[128];
				const int resolve_type = FEATURES::RAGEBOT::resolver.GetPlayerResolveInfo(entity).resolve_type;
				text_queue.emplace_back(resolve_state, FEATURES::RAGEBOT::resolver.GetResolveColor(FEATURES::RAGEBOT::resolver.GetPlayerResolveInfo(entity).resolve_type));

				if (FEATURES::RAGEBOT::resolver.GetPlayerResolveInfo(entity).is_balance_adjust_triggered)
					text_queue.emplace_back("Breaking LBY", GREEN);
			}

			/// draw everything on the list
			for (int i = 0; i < text_queue.size(); i++)
			{
				const auto col = CColor(text_queue[i].second.RGBA[0], text_queue[i].second.RGBA[1], text_queue[i].second.RGBA[2], text_color.RGBA[3]);
				RENDER::DrawF(top_right.x + 4, top_right.y + (i * 11), font, false, false, col, text_queue[i].first);
			}

			if (draw_name)
			{
				char player_name[128];
				memcpy_s(player_name, 128, UTILS::GetPlayerName(entity), 128);

				const size_t name_size = strlen(UTILS::GetPlayerName(entity));
				if (name_size > 16)
				{
					player_name[17] = '.';
					player_name[18] = '.';
					player_name[19] = '.';
					player_name[20] = '\0';
				}

				RENDER::DrawF(top_center.x, top_center.y - 6, font, true, true, text_color, player_name);
			}
			if (draw_weapon)
			{
				auto DrawWeaponIcon = [weapon, bottom_center, text_color]() -> bool
					{
						char icon_letter = '\0';

						switch (reinterpret_cast<SDK::CBaseWeapon*>(weapon)->GetItemDefinitionIndex())
						{
							case SDK::WEAPON_DEAGLE:
								icon_letter = 'F';
								break;
							case SDK::WEAPON_KNIFE:
							case SDK::WEAPON_KNIFE2:
								icon_letter = 'J';
								break;
							case SDK::WEAPON_SCOUT:
								icon_letter = 'R';
								break;
							case SDK::WEAPON_AUG:
								icon_letter = 'E';
								break;
							case SDK::WEAPON_SMOKE:
								icon_letter = 'G';
								break;
							case SDK::WEAPON_HEGRENADE:
								icon_letter = 'H';
								break;
							case SDK::WEAPON_G3SG1:
								icon_letter = 'I';
								break;
							case SDK::WEAPON_MAC10:
								icon_letter = 'L';
								break;
							case SDK::WEAPON_P90:
								icon_letter = 'M';
								break;
							case SDK::WEAPON_AWP:
								icon_letter = 'N';
								break;
							case SDK::WEAPON_SCAR20:
								icon_letter = 'O';
								break;
							case SDK::WEAPON_UMP:
								icon_letter = 'Q';
								break;
							case SDK::WEAPON_DUELIES:
								icon_letter = 'S';
								break;
							case SDK::WEAPON_FAMAS:
								icon_letter = 'T';
								break;
							case SDK::WEAPON_FIVESEVEN:
								icon_letter = 'U';
								break;
							case SDK::WEAPON_GALIL:
								icon_letter = 'V';
								break;
							case SDK::WEAPON_M4A4:
								icon_letter = 'W';
								break;
							case SDK::WEAPON_P250:
								icon_letter = 'Y';
								break;
							case SDK::WEAPON_M249:
								icon_letter = 'Z';
								break;
							case SDK::WEAPON_XM1014:
								icon_letter = ']';
								break;
							case SDK::WEAPON_C4:
								icon_letter = 'd';
								break;
							case SDK::WEAPON_GLOCK:
								icon_letter = 'C';
								break;
							case SDK::WEAPON_P2000:
								icon_letter = 'Y';
								break;
							default:
								return false;
						}

						char buffer[2] = {icon_letter, '\0'};
						RENDER::DrawF(bottom_center.x, bottom_center.y + 8, FONTS::weapon_icon_font, true, true, CColor(255, 255, 255, text_color.RGBA[3]), buffer);
						RENDER::DrawF(bottom_center.x + (RENDER::GetTextSize(FONTS::weapon_icon_font, buffer).x * 0.5), bottom_center.y + 9, FONTS::visuals_esp_font, false, true, CColor(255, 255, 255, text_color.RGBA[3]), std::to_string(reinterpret_cast<SDK::CBaseWeapon*>(weapon)->GetAmmo()));

						return true;
					};

				if (!DrawWeaponIcon())
				{
					char weapon_name[128];
					sprintf(weapon_name, "%s %i", weapon_clientclass->m_pNetworkName, reinterpret_cast<SDK::CBaseWeapon*>(weapon)->GetAmmo());

					RENDER::DrawF(bottom_center.x, bottom_center.y + 6, font, true, true, CColor(255, 255, 255, text_color.RGBA[3]), weapon_name);
				}
			}
		}

		void CBoxESP::DrawHealthBar(SDK::CBaseEntity* entity, Vector* corners, int alpha)
		{
			const auto top_left = corners[0];
			const auto bottom_left = corners[3];

			const int health_bar_width = max((corners[1].x - corners[0].x) / 40.f, 2.f);
			const int health_bar_distance = max(2, health_bar_width);

			const float filled_fraction = min(entity->GetHealth() / 100.f, 1.f);
			const auto health_color = CColor(MATH::Lerp(1.f - filled_fraction, 0, 255), MATH::Lerp(filled_fraction, 0, 255), 0, alpha);

			Vector2D direction;
			MATH::AngleVectors2D(MATH::CalcAngle2D(bottom_left, top_left), direction);
			const float height = filled_fraction * (bottom_left - top_left).Length();

			/// Setup the corner positions for the health bar rectangle and the background rectangle
			std::vector<SDK::Vertex_t> vertices_1, vertices_2;
			vertices_1.push_back(SDK::Vertex_t(Vector2D(top_left.x - health_bar_distance - health_bar_width, top_left.y)));
			vertices_1.push_back(SDK::Vertex_t(Vector2D(top_left.x - health_bar_distance, top_left.y)));
			vertices_1.push_back(SDK::Vertex_t(Vector2D(bottom_left.x - health_bar_distance, bottom_left.y)));
			vertices_1.push_back(SDK::Vertex_t(Vector2D(bottom_left.x - health_bar_distance - health_bar_width, bottom_left.y)));

			vertices_2.push_back(SDK::Vertex_t(Vector2D(bottom_left.x - health_bar_distance - health_bar_width, bottom_left.y) + (direction * height)));
			vertices_2.push_back(SDK::Vertex_t(Vector2D(bottom_left.x - health_bar_distance, bottom_left.y) + (direction * height)));
			vertices_2.push_back(SDK::Vertex_t(Vector2D(bottom_left.x - health_bar_distance, bottom_left.y)));
			vertices_2.push_back(SDK::Vertex_t(Vector2D(bottom_left.x - health_bar_distance - health_bar_width, bottom_left.y)));

			/// draw the vertices
			RENDER::TexturedPolygon(vertices_1.size(), vertices_1, CColor(50, 50, 50, alpha));
			RENDER::TexturedPolygon(vertices_2.size(), vertices_2, health_color);

			/// only draw health text if they aren't at full health
			if (entity->GetHealth() < 100)
			{
				const auto position = Vector2D(bottom_left.x - (health_bar_distance + (health_bar_width * 0.5f)), bottom_left.y) + (direction * (height + 5));
				RENDER::DrawF(position.x, position.y, FONTS::visuals_esp_font, true, true, CColor(255, 255, 255, alpha), std::to_string(entity->GetHealth()));
			}
		}
	}
}
