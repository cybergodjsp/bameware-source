#include "../includes.h"

#include "Misc.h"

#include "hitsound.h"

#include "../UTILS/interfaces.h"
#include "../UTILS/render.h"
#include "../UTILS/hooks.h"

#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/NetChannel.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CPrediction.h"
#include "../SDK/CBaseAnimState.h"

#include "../FEATURES/Configurations.h"

#include "../FEATURES/AntiAim.h"
#include "../FEATURES/Backtracking.h"
#include "../FEATURES/Autowall.h"

namespace FEATURES
{
	namespace MISC
	{
		bool __fastcall HookedSetupBones(void* ECX, void* EDX, matrix3x4_t* matrix, int num_bones, int bone_mask, float current_time)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			auto animstate = local_player->GetAnimState();
			if (!ECX || !matrix || !local_player || local_player->GetHealth() <= 0 || !animstate)
				return HOOKS::original_setup_bones(ECX, matrix, num_bones, bone_mask, current_time);

			static auto last_networked_origin = Vector(0, 0, 0);
			if (GLOBAL::should_send_packet)
				last_networked_origin = local_player->GetVecOrigin();

			local_player->SetAbsOrigin(last_networked_origin);
			return HOOKS::original_setup_bones(ECX, matrix, num_bones, bone_mask, current_time);
		}

		void Misc()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			auto renderable = reinterpret_cast<void*>(uintptr_t(local_player) + 0x4);
			if (!renderable)
				return;

			if (static bool is_init = false;
			!is_init
			)
			{
				is_init = true;
				HOOKS::setup_bones_hook_manager.Init(renderable);
				HOOKS::original_setup_bones = HOOKS::setup_bones_hook_manager.HookFunction<HOOKS::SetupBonesFn>(13, HookedSetupBones);
			}
			if (!HOOKS::setup_bones_hook_manager.IsHooked(13))
				HOOKS::original_setup_bones = HOOKS::setup_bones_hook_manager.HookFunction<HOOKS::SetupBonesFn>(13, HookedSetupBones);
		}

		void FixMovement(SDK::CUserCmd* cmd)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			Vector real_viewangles;
			INTERFACES::Engine->GetViewAngles(real_viewangles);

			Vector vecMove(cmd->forwardmove, cmd->sidemove, cmd->upmove);
			float speed = sqrt(vecMove.x * vecMove.x + vecMove.y * vecMove.y);

			Vector angMove;
			MATH::VectorAngles(vecMove, angMove);

			float yaw = DEG2RAD(cmd->viewangles.y - real_viewangles.y + angMove.y);

			cmd->forwardmove = cos(yaw) * speed;
			cmd->sidemove = sin(yaw) * speed;

			cmd->buttons &= ~IN_RIGHT;
			cmd->buttons &= ~IN_MOVERIGHT;
			cmd->buttons &= ~IN_LEFT;
			cmd->buttons &= ~IN_MOVELEFT;
			cmd->buttons &= ~IN_FORWARD;
			cmd->buttons &= ~IN_BACK;

			if (cmd->forwardmove > 0.f)
				cmd->buttons |= IN_FORWARD;
			else if (cmd->forwardmove < 0.f)
				cmd->buttons |= IN_BACK;

			if (cmd->sidemove > 0.f)
			{
				cmd->buttons |= IN_RIGHT;
				cmd->buttons |= IN_MOVERIGHT;
			}
			else if (cmd->sidemove < 0.f)
			{
				cmd->buttons |= IN_LEFT;
				cmd->buttons |= IN_MOVELEFT;
			}
		}

		void UpdateAngles(SDK::CUserCmd* cmd)
		{
			/// call at the end of createmove, updates real and fake angles, dont modify bSendPacket after this is called

			if (cmd->buttons & IN_ATTACK)
				return;

			if (GLOBAL::should_send_packet)
				GLOBAL::fake_angles = cmd->viewangles;
			else
				GLOBAL::real_angles = cmd->viewangles;
		}

		void ThirdpersonOV()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			if (local_player->GetHealth() <= 0)
			{
				if (SETTINGS::main_configs.thirdperson_enabled)
					local_player->SetObserverMode(5);

				return;
			}

			local_player->SetObserverMode(0);

			/// trace to get the correct distance
			auto GetCorrectDistance = [&local_player](float ideal_distance) -> float
				{
					Vector inverse_angles;
					INTERFACES::Engine->GetViewAngles(inverse_angles);

					inverse_angles.x *= -1.f , inverse_angles.y += 180.f;

					Vector direction;
					MATH::AngleVectors(inverse_angles, &direction);

					SDK::CTraceWorldOnly filter;
					SDK::trace_t trace;
					SDK::Ray_t ray;

					ray.Init(local_player->GetVecOrigin() + local_player->GetViewOffset(), (local_player->GetVecOrigin() + local_player->GetViewOffset()) + (direction * ideal_distance));
					INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

					return (ideal_distance * trace.flFraction) - 10.f;
				};

			*reinterpret_cast<bool*>(uintptr_t(INTERFACES::Input) + 0xA5) = SETTINGS::main_configs.thirdperson_enabled;

			/// thirdperson key
			if (SETTINGS::main_configs.thirdperson_keybind != -1 && UTILS::INPUT::input_handler.GetKeyState(SETTINGS::main_configs.thirdperson_keybind) & 1)
				SETTINGS::main_configs.thirdperson_enabled = !SETTINGS::main_configs.thirdperson_enabled;

			Vector angles;
			INTERFACES::Engine->GetViewAngles(angles);

			/// Input + 0xA8 + 0x8 is thirdperson distance
			angles.z = GetCorrectDistance(SETTINGS::main_configs.thirdperson_distance);
			*reinterpret_cast<Vector*>(uintptr_t(INTERFACES::Input) + 0xA8) = angles;
		}

		void ThirdpersonFSN()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			auto GetThirdpersonAngles = [&local_player]() -> Vector
				{
					switch (SETTINGS::main_configs.thirdperson_angle)
					{
						case 0:
							return GLOBAL::real_angles;
						case 1:
							return GLOBAL::fake_angles;
						case 2:
							return Vector(GLOBAL::real_angles.x, local_player->GetLowerBodyYaw(), 0.f);
						default:
							return GLOBAL::real_angles;
					}
				};

			/// if in thirdperson
			if (*reinterpret_cast<bool*>(uintptr_t(INTERFACES::Input) + 0xA5))
				*reinterpret_cast<Vector*>(uintptr_t(local_player) + OFFSETS::deadflag + 0x4) = GetThirdpersonAngles();
		}

		void Fakelag(SDK::CUserCmd* cmd)
		{
			GLOBAL::should_send_packet = true;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			auto net_channel = INTERFACES::Engine->GetNetChannel();
			if (!net_channel)
				return;

			GLOBAL::choke_amount = 1; // standing flag always 1 cuz fuck you
			if (!(local_player->GetFlags() & FL_ONGROUND)) // jumping flag
			{
				if (SETTINGS::main_configs.fakelag_adaptive_enabled)
					GLOBAL::choke_amount = ceil(64.f / (local_player->GetVelocity().Length2D() * INTERFACES::Globals->interval_per_tick)) + 1;
				else
					GLOBAL::choke_amount = static_cast<int>(SETTINGS::main_configs.fakelag_jumping);
			}
			else if (local_player->GetVelocity().Length2D() > 0.1 || UTILS::IsPressingMovementKeys(cmd)) // moving flag	
			{
				GLOBAL::choke_amount = static_cast<int>(SETTINGS::main_configs.fakelag_moving);

				if (SETTINGS::main_configs.fakelag_on_peek)
				{
					auto GetHitboxPosition = [local_player](int hitbox_index) -> Vector
						{
							if (local_player->GetIsDormant() || local_player->GetHealth() <= 0)
								return Vector(0, 0, 0);

							const auto pModel = local_player->GetModel();
							if (!pModel)
								return Vector(0, 0, 0);

							auto pStudioHdr = INTERFACES::ModelInfo->GetStudioModel(pModel);
							if (!pStudioHdr)
								return Vector(0, 0, 0);

							auto pSet = pStudioHdr->GetHitboxSet(0);
							if (!pSet)
								return Vector(0, 0, 0);

							if (hitbox_index >= pSet->numhitboxes || hitbox_index < 0)
								return Vector(0, 0, 0);

							auto hitbox = pSet->GetHitbox(hitbox_index);
							if (!hitbox)
								return Vector(0, 0, 0);

							const auto bone_matrix = local_player->GetBoneMatrix(hitbox->bone);
							const auto bone_pos = Vector(bone_matrix[0][3], bone_matrix[1][3], bone_matrix[2][3]);

							return ((hitbox->bbmax + hitbox->bbmin) * 0.5f) + bone_pos;
						};

					local_player->SetupBones(nullptr, 128, BONE_USED_BY_ANYTHING, UTILS::GetCurtime());
					const auto end_position_1 = GetHitboxPosition(18),
						end_position_2 = GetHitboxPosition(16); //18, 16

					for (int i = 0; i < 64; i++)
					{
						auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
						if (!entity || entity->GetIsDormant() || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam())
							continue;

						const auto damage_1 = FEATURES::RAGEBOT::autowall.CalculateDamage(entity->GetVecOrigin() + Vector(0, 0, 64.f), local_player->GetVecOrigin(),
						                                                                  entity, local_player, 1).damage, damage_2 = FEATURES::RAGEBOT::autowall.CalculateDamage(entity->GetVecOrigin() + Vector(0, 0, 64.f),
						                                                                                                                                                          local_player->GetVecOrigin(), entity, local_player, 1).damage;

						if (UTILS::Max<int>(damage_1, damage_2) > 0)
						{
							GLOBAL::choke_amount = 14;
							break;
						}
					}
				}
			}

			if (GLOBAL::is_fakewalking && GLOBAL::should_choke_packets)
				GLOBAL::choke_amount = 14;
			else if (GLOBAL::is_fakewalking)
				GLOBAL::choke_amount = 1;

			if (GLOBAL::do_max_fake_lag)
				GLOBAL::choke_amount = 14;
			else
				GLOBAL::choke_amount = UTILS::Clamp<int>(GLOBAL::choke_amount, 1, 14);

			if (net_channel->m_nChokedPackets >= min(14, GLOBAL::choke_amount) || (net_channel->m_nChokedPackets > 0 && GLOBAL::disable_fake_lag && !GLOBAL::is_fakewalking))
			{
				GLOBAL::disable_fake_lag = false;
				GLOBAL::should_send_packet = true;
			}
			else
			{
				GLOBAL::should_send_packet = false;
				GLOBAL::do_max_fake_lag = false;
			}
		}

		void Bhop(SDK::CUserCmd* cmd)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			GLOBAL::is_jumping = cmd->buttons & IN_JUMP;

			if (cmd->buttons & IN_JUMP && !(local_player->GetFlags() & FL_ONGROUND) && SETTINGS::main_configs.bhop_enabled)
				cmd->buttons &= ~IN_JUMP;
		}

		void AutoRevolver(SDK::CUserCmd* cmd)
		{
			if (!SETTINGS::main_configs.auto_revolver_enabled)
				return;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
			if (!weapon || weapon->GetItemDefinitionIndex() != SDK::WEAPON_REVOLVER)
				return;

			static float delay = 0.f;
			if (delay < 0.15f)
			{
				delay += INTERFACES::Globals->interval_per_tick;
				cmd->buttons |= IN_ATTACK;
			}
			else
				delay = 0.f;
		}

		void AutoPistol(SDK::CUserCmd* cmd)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
			if (!weapon)
				return;

			if (cmd->buttons & IN_ATTACK)
			{
				if (weapon->GetNextPrimaryAttack() > UTILS::GetCurtime())
					cmd->buttons &= ~IN_ATTACK;
			}
			if (cmd->buttons & IN_ATTACK2)
			{
				if (weapon->GetNextSecondaryAttack() > UTILS::GetCurtime())
					cmd->buttons &= ~IN_ATTACK2;
			}
		}

		void ClantagChanger()
		{
			auto SetClanTag = [](const char* tag, const char* name)
				{
					typedef void (__fastcall* SetClanTagFn)(const char*, const char*);
					static auto set_clan_tag = reinterpret_cast<SetClanTagFn>(UTILS::FindPattern(enc_char("engine.dll"), (PBYTE)enc_char("\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30"), enc_char("xxxxxxxxx????xxxxxx")));
					set_clan_tag(tag, name);
				};
			auto TextMarquee = [](std::string text, int speed) -> std::string
				{
					if (!text.size() || !speed)
						return text;

					speed = speed % text.size();
					for (int i = 0; i < speed; i++)
					{
						char back = text.back();
						text.pop_back();
						text.insert(text.begin(), back);
					}
					return text;
				};
			auto TextShift = [](const char* text, float speed) -> const char*
			
				{
					static constexpr float num_spaces = 10;
					const int str_len = strlen(text);

					if (!speed || str_len <= 0)
						return text;

					const int half_size = fmod(speed / num_spaces, str_len);
					char final_text[128];
					memcpy(final_text, text, half_size);

					int i = 0;
					for (; i < num_spaces - fmod(speed, num_spaces); i++)
						final_text[half_size + i] = ' ';

					final_text[half_size + i] = text[half_size];

					for (i = half_size + i + 1; i < str_len + half_size + num_spaces; i++)
						final_text[i] = ' ';

					final_text[i] = '\0';

					return final_text;
				};

			if (!SETTINGS::main_configs.clantag_changer_enabled)
				return;

			switch (SETTINGS::main_configs.clantag_changer_style)
			{
				case 0: /// static
					SetClanTag(SETTINGS::main_configs.clantag_changer_text, "");
					break;
				case 1: /// marquee
					SetClanTag(TextMarquee(SETTINGS::main_configs.clantag_changer_text, UTILS::GetCurtime() * 5.f * SETTINGS::main_configs.clantag_changer_speed).c_str(), "");
					break;
				case 2: /// shift
					SetClanTag(TextShift(SETTINGS::main_configs.clantag_changer_text, UTILS::GetCurtime() * 10.f * SETTINGS::main_configs.clantag_changer_speed), "");
					break;
			}
		}

		void Radar()
		{
			if (!SETTINGS::main_configs.radar_enabled)
				return;

			for (int i = 0; i < 64; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetHealth() <= 0)
					continue;

				entity->SetSpotted(true);
			}
		}

		void LoadUserConfig()
		{
			static const char file_path[MAX_PATH] = "C:\\Bameware\\user_settings.cfg";

			auto file = std::fstream(file_path, std::ios::in | std::ios::out);
			if (!file.is_open())
			{
				file = std::fstream(file_path, std::ios::in | std::ios::out | std::ios::trunc);
				if (!file.is_open())
					return;

				/// initialize the user settings template file
				file << "\\\\ These are options that are ran only once on injection" << std::endl;
				file << "\\\\ Use (ENABLED) and (DISABLED) to toggle something on or off" << std::endl;
				file << std::endl;

				file << "[Load config] (DISABLED) \\\\ ragebot config to be loaded on injection" << std::endl;
				file << "[Load visuals/misc config] (DISABLED) \\\\ visuals/misc config to be loaded on injection" << std::endl;
				file << "[Load skins on injection] (DISABLED)" << std::endl;

				file.seekg(0);
			}

			/// parses the file into seperate lines, with the lines made up of "tokens"
			auto ParseFile = [&file]() -> std::vector<std::vector<std::string>>
				{
					std::vector<std::vector<std::string>> file_contents;

					/// parse the line into tokens
					auto ParseLine = [](std::string line) -> std::vector<std::string>
						{
							std::vector<std::string> line_contents;

							if (line.size() < 3)
								return line_contents;

							/// do comments				
							for (int i = 0; i < line.size() - 1; i++)
							{
								if (line[i] == '\\' && line[i + 1] == '\\')
								{
									line.erase(line.begin() + i, line.end());
									break;
								}
							}

							if (line.size() < 3)
								return line_contents;

							/// get the values 
							{
								bool is_in_value_section = false;
								std::string value;
								for (int i = 0; i < line.size(); i++)
								{
									const char letter = line[i];
									if (letter == ')')
									{
										if (is_in_value_section)
											line_contents.push_back(value);

										is_in_value_section = false;
										value.clear();
									}

									if (is_in_value_section)
										value.push_back(letter);

									if (letter == '(')
										is_in_value_section = true;
								}
							}

							/// only get the name if contents were found
							if (line_contents.size())
							{
								bool is_in_name_section = false;
								std::string name;
								for (int i = 0; i < line.size(); i++)
								{
									const char letter = line[i];
									if (letter == ']')
										break;

									if (is_in_name_section)
										name.push_back(letter);

									if (letter == '[')
										is_in_name_section = true;
								}

								/// no name could be found
								if (!is_in_name_section)
								{
									line_contents.clear();
									return line_contents;
								}

								/// the name is always the first string in the vector of tokens
								line_contents.insert(line_contents.begin(), name);
							}

							return line_contents;
						};

					std::string current_line;
					while (std::getline(file, current_line))
					{
						const auto parsed_line = ParseLine(current_line);
						if (parsed_line.size())
							file_contents.push_back(parsed_line);
					}

					return file_contents;
				};

			/// do something with the parsed file...
			const auto contents = ParseFile();
			for (const auto& line : contents)
			{
				if (line.size() <= 0 || line[0].size() <= 1)
					continue;

				switch (UTILS::FNVHashEx(line[0].c_str()))
				{
					case UTILS::FNVHashEx("Load ragebot config"):
					{
						if (line[1] == "DISABLED")
							break;

						SETTINGS::ragebot_configs.Load(line[1].c_str(), sizeof(SETTINGS::RagebotConfig));
						break;
					}
					case UTILS::FNVHashEx("Load visuals/misc config"):
					{
						if (line[1] == "DISABLED")
							break;

						SETTINGS::main_configs.Load(line[1].c_str(), sizeof(SETTINGS::MainConfig));

						break;
					}
					case UTILS::FNVHashEx("Load skins on injection"):
					{
						if (line[1] == "ENABLED")
							SETTINGS::skin_configs.Load("skins_config.scfg", sizeof(SETTINGS::SkinConfig));

						break;
					}
				}
			}

			file.close();
		}

		void DrawMisc()
		{
			int screen_width, screen_height;
			INTERFACES::Engine->GetScreenSize(screen_width, screen_height);

			auto DoBamewareInto = [screen_width, screen_height]()
				{
					static constexpr float intro_length_time = 2.f, intro_max_height_offset = -25.f, intro_max_width_offset = -15.f;

					const float current_time = GetTickCount() * 0.001f, time_delta = current_time - GLOBAL::cheat_start_time;
					if (time_delta < intro_length_time)
					{
						static constexpr char bameware_text[] = "Bameware ";

						const int text_width = RENDER::GetTextSize(FONTS::welcome_font, bameware_text).x, text_len = strlen(bameware_text);
						const int center_width = screen_width * 0.5, center_height = screen_height * 0.5;
						const int current_letter = ((time_delta / intro_length_time) * text_len) + 1;
						const float time_per_letter = intro_length_time / static_cast<float>(text_len);
						for (int i = 0; i < text_len; i++)
						{
							if (i >= current_letter)
								continue;

							const float frac = ((fmod(time_delta, time_per_letter) * 0.5f + (i == current_letter - 2 ? time_per_letter * 0.5f : 0)) / time_per_letter);

							int alpha = 0;
							if (current_letter > i + 2)
								alpha = 255;
							else
								alpha = frac * 255.f;

							const int cur_width = center_width + (text_width * 0.5f) - (text_width / static_cast<float>(text_len)) * static_cast<float>(text_len - i);
							if (i == current_letter - 1 || i == current_letter - 2)
							{
								const float frac_2 = (frac > 0.5f ? 1.f - frac : frac) * 2.f;
								RENDER::DrawF(cur_width + (frac_2 * intro_max_width_offset), center_height + (frac_2 * intro_max_height_offset), FONTS::welcome_font, false, true, CColor(255, 20, 147, alpha), std::string(1, bameware_text[i]));
							}
							else
								RENDER::DrawF(cur_width, center_height, FONTS::welcome_font, false, true, CColor(255, 20, 147, alpha), std::string(1, bameware_text[i]));
						}
					}
				};

			DoBamewareInto();

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

			if (local_player && local_player->GetHealth() > 0)
			{
				float breaking_lby_fraction = fabs(MATH::NormalizeYaw(GLOBAL::real_angles.y - local_player->GetLowerBodyYaw())) / 180.f;
				RENDER::DrawF(30, screen_height - 70, FONTS::lby_indicator_font, false, false, CColor((1.f - breaking_lby_fraction) * 255.f, breaking_lby_fraction * 255.f, 0), "LBY");
			}
		}

		BulletTracers bullet_tracers;

		void BulletTracers::Do()
		{
			if (!tracers.size())
				return;

			if (fabs(UTILS::GetCurtime() - tracers.back().m_time) > max_bullet_trace_time)
				tracers.pop_back();

			for (auto tracer : tracers)
			{
				Vector screen1, screen2;
				if (!RENDER::WorldToScreen(tracer.m_start, screen1) || !RENDER::WorldToScreen(tracer.m_end, screen2))
					continue;

				RENDER::DrawLine(screen1.x, screen1.y, screen2.x, screen2.y, tracer.m_color);
			}
		}

		Hitmarkers hitmarkers;

		void Hitmarkers::Do()
		{
			if (!hitmarker_vector.size())
				return;

			if (fabs(UTILS::GetCurtime() - hitmarker_vector.back().m_time) > hitmarker_damage_fade_time)
			{
				hitmarker_vector.pop_back();

				if (!hitmarker_vector.size())
					return;
			}

			int width, height;
			INTERFACES::Engine->GetScreenSize(width, height);

			int center_width = width * 0.5f, center_height = height * 0.5f;

			for (const auto hitmarker : hitmarker_vector)
			{
				const float time_delta = fabs(UTILS::GetCurtime() - hitmarker.m_time);
				const float extra_height = (time_delta / hitmarker_damage_fade_time) * hitmarker_damage_float_height;

				const int alpha = 255 - ((time_delta / hitmarker_damage_fade_time) * 255.f);
				const CColor color = CColor(min(255, (hitmarker.m_damage * 0.01f) * 255.f), max(0, 255 - ((hitmarker.m_damage * 0.01f) * 255.f)), 0, alpha);

				RENDER::DrawF(center_width + 75 + (sin(time_delta * 2.5f) * 60.f), center_height - extra_height, FONTS::visuals_esp_font, true, true, color, std::to_string(hitmarker.m_damage));
			}

			const auto hitmarker = hitmarker_vector.front();

			const float time_delta = fabs(UTILS::GetCurtime() - hitmarker.m_time);
			if (time_delta > hitmarker_fade_time)
				return;

			const int alpha = 125 - ((time_delta / hitmarker_fade_time) * 125.f);
			const CColor color = CColor(255, 255, 255, alpha);

			RENDER::DrawLine(center_width + 4, center_height + 4, center_width + 7, center_height + 7, color);
			RENDER::DrawLine(center_width - 4, center_height + 4, center_width - 7, center_height + 7, color);
			RENDER::DrawLine(center_width + 4, center_height - 4, center_width + 7, center_height - 7, color);
			RENDER::DrawLine(center_width - 4, center_height - 4, center_width - 7, center_height - 7, color);
		}

		void Hitmarkers::AddHitmarker(int damage)
		{
			hitmarker_vector.insert(hitmarker_vector.begin(), Hitmarker(UTILS::GetCurtime(), damage));
			PlaySoundA(reinterpret_cast<const char*>(hitsound_byte_array), nullptr, SND_MEMORY | SND_ASYNC);
		}
	}
}
