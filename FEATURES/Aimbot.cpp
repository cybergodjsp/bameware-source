#include "../includes.h"

#include "Aimbot.h"

#include "../UTILS/interfaces.h"
#include "../UTILS/render.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/IEngine.h"
#include "../SDK/CTrace.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/NetChannel.h"
#include "../SDK/CGlobalVars.h"

#include "../FEATURES/Backtracking.h"
#include "../FEATURES/Resolver.h"
#include "../FEATURES/Autowall.h"

#include "../UTILS/Debugger.h"
#include "../FEATURES/InGameLogger.h"

#include "../FEATURES/Configurations.h"

namespace FEATURES
{
	namespace RAGEBOT
	{
		Aimbot aimbot;

		void Aimbot::Do(SDK::CUserCmd* cmd)
		{
			GLOBAL::can_shoot_someone = false;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0 || !SETTINGS::ragebot_configs.aimbot_enabled ||
				UTILS::IsWeaponKnife(UTILS::GetWeapon(local_player)) || UTILS::IsWeaponGrenade(UTILS::GetWeapon(local_player)))
				return;

			auto BoundingBoxCheck = [this, local_player](SDK::CBaseEntity* entity, const Backtracking_Record& record) -> bool
				{
					const auto bbmin = record.bbmin + record.vec_origin;
					const auto bbmax = record.bbmax + record.vec_origin;

					/// the 4 corners on the top, 1 for the head, 1 for the middle of the body, 1 for the feet
					Vector points[7];

					points[0] = GetHitboxPosition(entity, 0);
					points[1] = (bbmin + bbmax) * 0.5f;
					points[2] = Vector((bbmax.x + bbmin.x) * 0.5f, (bbmax.y + bbmin.y) * 0.5f, bbmin.z);

					points[3] = bbmax;
					points[4] = Vector(bbmax.x, bbmin.y, bbmax.z);
					points[5] = Vector(bbmin.x, bbmin.y, bbmax.z);
					points[6] = Vector(bbmin.x, bbmax.y, bbmax.z);

					for (const auto& point : points)
					{
						if (autowall.CalculateDamage(local_player->GetVecOrigin() + local_player->GetViewOffset(), point, local_player, entity).damage > 0)
							return true;
					}

					return false;
				};

			/// loop through every entity to find valid ones to aimbot
			for (int i = 0; i < 64; i++)
			{
				/// reset player aimbot info
				player_aimbot_info[i].head_damage = -1 , player_aimbot_info[i].hitscan_damage = -1;

				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetIsDormant() || entity->GetImmunity() || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam())
					continue;

				int most_damage = 0;
				for (const auto& rec : backtracking.GetPriorityRecords(entity))
				{
					const int tick = backtracking.GetTickCount(rec);

					/// optimization
					if (!BoundingBoxCheck(entity, rec))
						continue;

					int random_ticks = 0;
					if (rec.is_exploiting)
						random_ticks = TIME_TO_TICKS(UTILS::GetLatency());

					backtracking.Restore(entity, rec, random_ticks);
					backtracking.ApplyRestore(entity, TICKS_TO_TIME(tick));

					int cur_head_damage, cur_hitscan_damage;
					Vector cur_head_pos, cur_hitscan_pos;

					DoHeadAim(entity, cur_head_pos, cur_head_damage);
					DoHitscan(entity, cur_hitscan_pos, cur_hitscan_damage);

					if (cur_head_damage > most_damage || cur_hitscan_damage > most_damage)
					{
						most_damage = UTILS::Max<int>(cur_head_damage, cur_hitscan_damage);

						player_aimbot_info[i].head_damage = cur_head_damage;
						player_aimbot_info[i].hitscan_damage = cur_hitscan_damage;

						player_aimbot_info[i].head_position = cur_head_pos;
						player_aimbot_info[i].hitscan_position = cur_hitscan_pos;

						player_aimbot_info[i].tick = tick;
						player_aimbot_info[i].backtrack_record = rec;
						player_aimbot_info[i].extrapolted_ticks = random_ticks;

						if (cur_hitscan_damage > entity->GetHealth())
							break;
					}
				}
			}

			int highest_damage = 0, best_tick, last_extrapolated_ticks;
			FEATURES::RAGEBOT::Backtracking_Record last_backtrack_record;
			SDK::CBaseEntity* best_entity = nullptr;
			bool is_hitscan = false;
			Vector best_position;

			/// sort valid entities by most damage
			for (int i = 0; i < 64; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity)
					continue;

				auto FillInfo = [&is_hitscan, &best_position, &best_entity, &last_backtrack_record, &last_extrapolated_ticks, &highest_damage, &best_tick, entity, i, this](bool hitscan) -> void
					{
						is_hitscan = hitscan , best_tick = player_aimbot_info[i].tick;
						best_entity = entity , last_backtrack_record = player_aimbot_info[i].backtrack_record;
						last_extrapolated_ticks = player_aimbot_info[i].extrapolted_ticks;

						if (hitscan)
						{
							best_position = player_aimbot_info[i].hitscan_position;
							highest_damage = player_aimbot_info[i].hitscan_damage;
						}
						else
						{
							best_position = player_aimbot_info[i].head_position;
							highest_damage = player_aimbot_info[i].head_damage;
						}
					};
				const int hitscan_dmg = player_aimbot_info[i].hitscan_damage, head_dmg = player_aimbot_info[i].head_damage;

				/// if exploiting or jumping force baim and entirely ignore head
				const bool is_exploiting = player_aimbot_info[i].backtrack_record.is_exploiting;
				if (is_exploiting || (!(entity->GetFlags() & FL_ONGROUND)))
				{
					if (hitscan_dmg > highest_damage)
						FillInfo(true);

					continue;
				}

				/// can't shoot this entity, continue
				if (hitscan_dmg <= 0 && head_dmg <= 0)
					continue;

				/// one of these is not valid, choose the one that is valid
				if (hitscan_dmg <= 0 || head_dmg <= 0)
				{
					FillInfo(hitscan_dmg > head_dmg);
					continue;
				}

				/// hitscan if lethal or if more than head damage
				if (hitscan_dmg > entity->GetHealth() || hitscan_dmg > head_dmg)
				{
					FillInfo(true);
					if (hitscan_dmg > entity->GetHealth())
						break;

					continue;
				}

				/// if resolved go for head
				if (FEATURES::RAGEBOT::resolver.IsResolved(player_aimbot_info[i].backtrack_record.resolve_record.resolve_type))
				{
					FillInfo(false);
					break;
				}

				const bool baim_if_unplugged = (SETTINGS::ragebot_configs.aimbot_baim_if_unplugged && fabs(UTILS::GetCurtime() - player_aimbot_info[i].backtrack_record.resolve_record.last_time_moving) > 1.f);
				const bool prefer_baim = SETTINGS::ragebot_configs.aimbot_prefer_baim_enabled || !(entity->GetFlags() & FL_ONGROUND) || (SETTINGS::ragebot_configs.aimbot_bodyaim_if_x_damage < player_aimbot_info[i].hitscan_damage);
				if (prefer_baim || baim_if_unplugged)
				{
					FillInfo(true);

					continue;
				}

				FillInfo(false);
			}

			static float duration_spent_aiming = 0.f;

			/// we have a valid target to shoot at
			if (best_entity)
			{
				GLOBAL::can_shoot_someone = true;

				duration_spent_aiming += INTERFACES::Globals->interval_per_tick;

				/// maybe implement a knifebot sometime in the future?
				const bool should_right_click = false;

				/// the weapon is ready to fire
				if (UTILS::CanShoot(should_right_click))
				{
					/// autoscope
					if (!local_player->IsScopedIn() && UTILS::CanShoot(true) && UTILS::IsWeaponSniper(UTILS::GetWeapon(local_player)) && local_player->GetFlags() & FL_ONGROUND)
						cmd->buttons |= IN_ATTACK2;

					/// need to restore to the player's backtrack record so that hitchance can work properly
					backtracking.Restore(best_entity, last_backtrack_record, last_extrapolated_ticks);
					backtracking.ApplyRestore(best_entity, best_tick);

					/// delay shot
					auto DelayShot = [best_entity, last_backtrack_record, highest_damage, local_player, is_hitscan]() -> bool
						{
							/// jumping
							if (!(best_entity->GetFlags() & FL_ONGROUND))
								return true;

							if (highest_damage >= best_entity->GetHealth() + 10)
								return true;

							if (best_entity->GetVelocity().Length2D() > 75.f || local_player->GetVelocity().Length2D() > 75.f) /// moving fast
								return duration_spent_aiming >= SETTINGS::ragebot_configs.aimbot_delay_shot;

							return true;
						};
					if (DelayShot())
					{
						if (SETTINGS::ragebot_configs.aimbot_autostop_enabled && local_player->GetFlags() & FL_ONGROUND)
						{
							cmd->sidemove = 0;
							cmd->forwardmove = local_player->GetVelocity().Length2D() > 20.f ? 450.f : 0.f;

							UTILS::RotateMovement(cmd, MATH::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y + 180.f);
						}

						/// if is accurate enough to shoot, then shoot... (hitchance, spread limit, etc)
						if (const auto net_channel = INTERFACES::Engine->GetNetChannel(); IsAccurate(best_entity, best_position) && net_channel && net_channel->m_nChokedPackets)
						{
							if (last_backtrack_record.is_exploiting)
							{
								FEATURES::MISC::InGameLogger::Log log;
								log.color_line.PushBack(enc_str("Shot at exploiting player!"), BLUE);
								FEATURES::MISC::in_game_logger.AddLog(log);
							}

							if (should_right_click)
								cmd->buttons |= IN_ATTACK2;
							else
								cmd->buttons |= IN_ATTACK;

							cmd->viewangles = MATH::CalcAngle(local_player->GetVecOrigin() + local_player->GetViewOffset(), best_position) - (local_player->GetPunchAngles() * 2.f);
							cmd->tick_count = best_tick;

							/// resolver
							auto IsSidewaysAntiaim = [is_hitscan, best_entity, local_player, last_backtrack_record]() -> bool
								{
									const float at_target_yaw = MATH::NormalizeYaw(MATH::CalcAngle(local_player->GetVecOrigin(), best_entity->GetVecOrigin()).y);

									const float delta_1 = fabs(MATH::NormalizeYaw((at_target_yaw + 90.f) - last_backtrack_record.eye_angles.y)),
										delta_2 = fabs(MATH::NormalizeYaw((at_target_yaw - 90.f) - last_backtrack_record.eye_angles.y));

									return UTILS::Min<float>(delta_1, delta_2) > 30.f;
								};
							if (!is_hitscan || IsSidewaysAntiaim())
								FEATURES::RAGEBOT::resolver.AddShotSnapshot(best_entity, FEATURES::RAGEBOT::resolver.GetPlayerResolveInfo(best_entity));

							/// psilent
							GLOBAL::should_send_packet = false;
							GLOBAL::disable_fake_lag = true;
						}
					}
					else if (SETTINGS::ragebot_configs.aimbot_autostop_enabled && local_player->GetFlags() & FL_ONGROUND)
					{
						const int ticks_to_stop = UTILS::TicksToStop(local_player->GetVelocity());
						const auto new_eye_position = local_player->GetVecOrigin() + local_player->GetViewOffset() +
							(local_player->GetVelocity() * INTERFACES::Globals->interval_per_tick * ticks_to_stop);

						if (FEATURES::RAGEBOT::autowall.CalculateDamage(new_eye_position, GetHitboxPosition(best_entity, 0), local_player, best_entity).damage > best_entity->GetHealth())
						{
							cmd->sidemove = 0;
							cmd->forwardmove = local_player->GetVelocity().Length2D() > 13.f ? 450.f : 0.f;

							UTILS::RotateMovement(cmd, MATH::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y + 180.f);
							std::cout << ticks_to_stop << std::endl;
						}
					}
				}
				else if (const auto weapon = UTILS::GetWeapon(local_player); weapon && weapon->GetItemDefinitionIndex() == SDK::WEAPON_REVOLVER)
						cmd->buttons |= IN_ATTACK;
			}

			/// restore all players back to their current record
			for (int i = 0; i < 64; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetIsDormant() || entity->GetImmunity() || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam())
					continue;

				backtracking.RestoreToCurrentRecord(entity);
			}

			if (!best_entity)
				duration_spent_aiming = 0.f;
		}

		bool Aimbot::DoHitscan(SDK::CBaseEntity* entity, Vector& final_position, int& final_damage)
		{
			/// default value
			final_damage = 0;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			auto weapon = UTILS::GetWeapon(local_player);
			if (!weapon)
				return false;

			const auto eye_position = local_player->GetVecOrigin() + local_player->GetViewOffset();
			const bool is_taser = UTILS::IsWeaponTaser(weapon);
			const bool should_ignore_limbs = entity->GetVelocity().Length2D() > 30.f;
			const float pointscale = SETTINGS::ragebot_configs.aimbot_hitscan_pointscale * 0.01f;

			/// if taser or low on ammo, set minimum damage to their health so that it kills in a single shot
			const int minimum_damage = (is_taser || weapon->GetAmmo() <= 2) ? entity->GetHealth() + 10 : UTILS::Min<int>(SETTINGS::ragebot_configs.aimbot_minimum_damage, entity->GetHealth() + 10);
			const int minimum_autowall_damage = (is_taser || weapon->GetAmmo() <= 2) ? entity->GetHealth() + 10 : UTILS::Min<int>(SETTINGS::ragebot_configs.aimbot_minimum_autowall_damage, entity->GetHealth() + 10);

			/// functions for creating the hitscan positions
			auto ConstructHitscanMultipointPositions = [this, entity, pointscale, eye_position, local_player](bool ignore_limbs) -> std::vector<std::pair<Vector, int>>
				{
					std::vector<std::pair<Vector, int>> positions;

					auto CreateAndAddBufferToVec = [this, entity](std::vector<std::pair<Vector, int>>& vec, int hitbox_index, float pointscale)
						{
							std::vector<Vector> buffer;
							GetMultipointPositions(entity, buffer, hitbox_index, pointscale);

							for (const auto& x : buffer)
								vec.emplace_back(x, hitbox_index);
						};
					auto GetSpreadDistance = [this, local_player](float distance) -> float
						{
							auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
							if (!weapon)
								return 0.f;

							Vector viewangles;
							INTERFACES::Engine->GetViewAngles(viewangles);

							weapon->UpdateAccuracyPenalty();
							float spread = weapon->GetSpreadCone();
							float inaccuracy = weapon->GetInnaccuracy();

							Vector forward, right, up;
							MATH::AngleVectors(viewangles, &forward, &right, &up);

							float RandomA = 0;
							float RandomB = 1.0f - RandomA * RandomA;

							RandomA = M_PI_F * 2.0f;
							RandomB *= spread;

							float SpreadX1 = (cos(RandomA) * RandomB);
							float SpreadY1 = (sin(RandomA) * RandomB);

							float RandomC = 0;
							float RandomF = 1.0f - RandomC * RandomC;

							RandomC = M_PI_F * 2.0f;
							RandomF *= inaccuracy;

							float SpreadX2 = (cos(RandomC) * RandomF);
							float SpreadY2 = (sin(RandomC) * RandomF);

							float fSpreadX = SpreadX1 + SpreadX2;
							float fSpreadY = SpreadY1 + SpreadY2;

							Vector vSpreadForward;
							vSpreadForward[0] = forward[0] + (fSpreadX * right[0]) + (fSpreadY * up[0]);
							vSpreadForward[1] = forward[1] + (fSpreadX * right[1]) + (fSpreadY * up[1]);
							vSpreadForward[2] = forward[2] + (fSpreadX * right[2]) + (fSpreadY * up[2]);
							vSpreadForward.NormalizeInPlace();

							Vector qaNewAngle;
							MATH::VectorAngles(vSpreadForward, qaNewAngle);
							qaNewAngle = MATH::NormalizeAngle(qaNewAngle);

							Vector vEnd;
							MATH::AngleVectors(qaNewAngle, &vEnd);

							return ((vEnd - forward) * distance).Length();
						};
					auto GetIdealPointscale = [this, entity, eye_position, GetSpreadDistance](int hitbox_index) -> float
						{
							const auto hitbox = GetHitbox(entity, hitbox_index);
							if (!hitbox)
								return 0.f;

							const float hitbox_radius = hitbox->m_flRadius,
								spread_distance = GetSpreadDistance((eye_position - entity->GetVecOrigin()).Length());

							float ideal_pointscale = 0.f;
							if (spread_distance > hitbox_radius * 0.5f)
								return ideal_pointscale;

							ideal_pointscale = ((hitbox_radius * 0.5f) - spread_distance) / (hitbox_radius * 0.5f);

							return UTILS::Clamp<float>(ideal_pointscale - 0.1f, 0.f, 0.8f);
						};

					if (SETTINGS::ragebot_configs.aimbot_selected_multipoint_hitgroups[0]) /// chest
					{
						CreateAndAddBufferToVec(positions, 6, GetIdealPointscale(6));
						CreateAndAddBufferToVec(positions, 5, GetIdealPointscale(5));
					}
					if (SETTINGS::ragebot_configs.aimbot_selected_multipoint_hitgroups[1]) /// stomach
					{
						CreateAndAddBufferToVec(positions, 4, GetIdealPointscale(4));
						CreateAndAddBufferToVec(positions, 3, GetIdealPointscale(3));
					}
					if (SETTINGS::ragebot_configs.aimbot_selected_multipoint_hitgroups[2]) /// arms
					{
						CreateAndAddBufferToVec(positions, 18, GetIdealPointscale(18));
						CreateAndAddBufferToVec(positions, 16, GetIdealPointscale(16));
					}
					if (SETTINGS::ragebot_configs.aimbot_selected_multipoint_hitgroups[3] && !ignore_limbs) /// legs
					{
						CreateAndAddBufferToVec(positions, 9, GetIdealPointscale(9));
						CreateAndAddBufferToVec(positions, 8, GetIdealPointscale(8));
					}

					return positions;
				};
			auto ConstructHitscanPositions = [this, entity](bool ignore_limbs) -> std::vector<std::pair<Vector, int>>
				{
					std::vector<std::pair<Vector, int>> positions;

					if (SETTINGS::ragebot_configs.aimbot_selected_hitgroups[0]) /// chest
					{
						positions.emplace_back(GetHitboxPosition(entity, 6), 6);
						positions.emplace_back(GetHitboxPosition(entity, 5), 5);
					}
					if (SETTINGS::ragebot_configs.aimbot_selected_hitgroups[1]) /// stomach
					{
						positions.emplace_back(GetHitboxPosition(entity, 4), 4);
						positions.emplace_back(GetHitboxPosition(entity, 3), 3);
					}
					if (SETTINGS::ragebot_configs.aimbot_selected_hitgroups[2]) /// arms
					{
						positions.emplace_back(GetHitboxPosition(entity, 18), 18);
						positions.emplace_back(GetHitboxPosition(entity, 16), 16);
					}
					if (SETTINGS::ragebot_configs.aimbot_selected_hitgroups[3] && !ignore_limbs) /// legs
					{
						positions.emplace_back(GetHitboxPosition(entity, 9), 9);
						positions.emplace_back(GetHitboxPosition(entity, 8), 8);
					}
					if (SETTINGS::ragebot_configs.aimbot_selected_hitgroups[4] && !ignore_limbs) /// feet
					{
						positions.emplace_back(GetHitboxPosition(entity, 13), 13);
						positions.emplace_back(GetHitboxPosition(entity, 12), 12);
						positions.emplace_back(GetHitboxPosition(entity, 11), 11);
						positions.emplace_back(GetHitboxPosition(entity, 10), 10);
					}

					return positions;
				};

			/// get the positions
			const auto hitscan_mp_positions = ConstructHitscanMultipointPositions(should_ignore_limbs);
			const auto hitscan_positions = ConstructHitscanPositions(should_ignore_limbs);

			/// get the best multipoint hitscan position
			int mp_damage = -1, mp_min_damage = 0;
			Vector mp_position;
			for (const auto& pair : hitscan_mp_positions)
			{
				const auto info = FEATURES::RAGEBOT::autowall.CalculateDamage(eye_position, pair.first, local_player, entity, is_taser ? 0 : -1);
				const int min_dmg = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;

				if (info.damage > mp_damage)
				{
					mp_damage = info.damage , mp_position = pair.first;
					mp_min_damage = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;
				}
			}

			/// get the best hitscan position
			int hitscan_damage = -1, hitscan_min_damage = 0, hitscan_hitbox = 0;
			Vector hitscan_position;
			for (const auto& pair : hitscan_positions)
			{
				const auto info = FEATURES::RAGEBOT::autowall.CalculateDamage(eye_position, pair.first, local_player, entity, is_taser ? 0 : -1);
				const bool has_pos_already = hitscan_damage != -1;

				if (info.damage > hitscan_damage)
				{
					/// feet are the least priority
					//const bool is_feet = pair.second >= 10 && pair.second <= 13;
					//if (is_feet && has_pos_already)
					//continue;

					hitscan_hitbox = pair.second;
					hitscan_damage = info.damage , hitscan_position = pair.first;
					hitscan_min_damage = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;
				}
			}

			/// no positions deal enough damage
			if (hitscan_damage < hitscan_min_damage && mp_damage < mp_min_damage)
				return false;

			/// one of these are not valid, choose the one that is valid
			if (hitscan_damage < hitscan_min_damage || mp_damage < mp_min_damage)
			{
				if (hitscan_damage >= hitscan_min_damage)
				{
					final_position = hitscan_position;
					final_damage = hitscan_damage;
					return true;
				}
				else
				{
					final_position = mp_position;
					final_damage = mp_damage;
					return true;
				}
			}

			/// just choose the position that does most damage gg
			auto PrioritiseHitscan = [mp_damage, hitscan_hitbox, hitscan_damage, entity]() -> bool
				{
					switch (hitscan_hitbox)
					{
						case 6:
						case 5:
						case 4:
						case 3:
						{
							/// is lethal
							if (hitscan_damage > entity->GetHealth() + 10)
								return true;

							if (mp_damage > entity->GetHealth())
								return false;

							/// less than 10 damage difference, prefer hitscan over multipoint 
							if (abs(mp_damage - hitscan_damage) < 10)
								return true;
						}
						default:
							return false;
					}
				};
			if (hitscan_damage > mp_damage || PrioritiseHitscan())
			{
				final_position = hitscan_position;
				final_damage = hitscan_damage;
			}
			else
			{
				final_position = mp_position;
				final_damage = mp_damage;
			}

			return true;
		}

		bool Aimbot::DoHeadAim(SDK::CBaseEntity* entity, Vector& final_position, int& final_damage)
		{
			/// default value
			final_damage = 0;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			auto weapon = UTILS::GetWeapon(local_player);
			if (!weapon)
				return false;

			if (UTILS::IsWeaponTaser(weapon))
				return false;

			const auto eye_position = local_player->GetVecOrigin() + local_player->GetViewOffset();
			const float pointscale = SETTINGS::ragebot_configs.aimbot_pointscale * 0.01f;

			/// if taser or low on ammo, set minimum damage to their health so that it kills in a single shot
			const int minimum_damage = (weapon->GetAmmo() <= 2) ? entity->GetHealth() : UTILS::Min<int>(SETTINGS::ragebot_configs.aimbot_minimum_damage, entity->GetHealth() + 10);
			const int minimum_autowall_damage = (weapon->GetAmmo() <= 2) ? entity->GetHealth() : UTILS::Min<int>(SETTINGS::ragebot_configs.aimbot_minimum_autowall_damage, entity->GetHealth() + 10);

			/// center of the hitbox
			const auto head_center_position = GetHitboxPosition(entity, 0);
			const auto head_center_info = FEATURES::RAGEBOT::autowall.CalculateDamage(eye_position, head_center_position, local_player, entity);
			const int head_center_damage = head_center_info.damage,
				head_center_min_damage = head_center_info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;

			/// the multipoints around the center of the hitbox
			std::vector<Vector> head_multipoints;
			GetMultipointPositions(entity, head_multipoints, 0, pointscale);

			/// get the multipoint position that does the most damage
			Vector best_multipoint_position;
			int best_multipoint_damage = -1, multipoint_min_damage = 0;
			for (const auto& x : head_multipoints)
			{
				const auto info = FEATURES::RAGEBOT::autowall.CalculateDamage(eye_position, x, local_player, entity);

				if (info.damage > best_multipoint_damage)
				{
					best_multipoint_damage = info.damage , best_multipoint_position = x;
					multipoint_min_damage = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;
				}
			}

			/// none are valid
			if (head_center_damage < minimum_damage && best_multipoint_damage < minimum_damage)
				return false;

			/// one of these aren't valid, choose the valid one
			if (head_center_damage < head_center_min_damage || best_multipoint_damage < multipoint_min_damage)
			{
				if (head_center_damage > best_multipoint_damage)
				{
					final_position = head_center_position;
					final_damage = head_center_damage;
				}
				else
				{
					final_position = best_multipoint_position;
					final_damage = best_multipoint_damage;
				}
			}
			else /// both are valid, do some more decision making 
			{
				/// decide whether to shoot the center of the hitbox or the multipoints of the hitbox
				if (head_center_damage > entity->GetHealth() + 10 || head_center_damage + 10 > best_multipoint_damage)
				{
					final_position = head_center_position;
					final_damage = head_center_damage;
				}
				else
				{
					final_position = best_multipoint_position;
					final_damage = best_multipoint_damage;
				}
			}

			return true;
		}

		bool Aimbot::IsAccurate(SDK::CBaseEntity* entity, Vector position)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			auto weapon = UTILS::GetWeapon(local_player);
			if (!weapon)
				return false;

			if (UTILS::IsWeaponKnife(weapon) || UTILS::IsWeaponTaser(weapon))
				return true;

			switch (SETTINGS::ragebot_configs.aimbot_accuracy_type)
			{
				case 0: /// none
					return true;
				case 1: /// hitchance
				{
					auto RandomFloat = [](float min, float max) -> float
						{
							typedef float (*RandomFloat_t)(float, float);
							static RandomFloat_t m_RandomFloat = (RandomFloat_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat");
							return m_RandomFloat(min, max);
						};

					Vector angle = MATH::NormalizeAngle(MATH::CalcAngle(local_player->GetVecOrigin() + local_player->GetViewOffset(), position));

					Vector forward, right, up;
					MATH::AngleVectors(angle, &forward, &right, &up);

					int iHit = 0;
					weapon->UpdateAccuracyPenalty();
					for (int i = 0; i < 256; i++)
					{
						float RandomA = RandomFloat(0.0f, 1.0f);
						float RandomB = 1.0f - RandomA * RandomA;

						RandomA = RandomFloat(0.0f, M_PI_F * 2.0f);
						RandomB *= weapon->GetSpreadCone();

						float SpreadX1 = (cos(RandomA) * RandomB);
						float SpreadY1 = (sin(RandomA) * RandomB);

						float RandomC = RandomFloat(0.0f, 1.0f);
						float RandomF = 1.0f - RandomC * RandomC;

						RandomC = RandomFloat(0.0f, M_PI_F * 2.0f);
						RandomF *= weapon->GetInnaccuracy();

						float SpreadX2 = (cos(RandomC) * RandomF);
						float SpreadY2 = (sin(RandomC) * RandomF);

						float fSpreadX = SpreadX1 + SpreadX2;
						float fSpreadY = SpreadY1 + SpreadY2;

						Vector vSpreadForward;
						vSpreadForward[0] = forward[0] + (fSpreadX * right[0]) + (fSpreadY * up[0]);
						vSpreadForward[1] = forward[1] + (fSpreadX * right[1]) + (fSpreadY * up[1]);
						vSpreadForward[2] = forward[2] + (fSpreadX * right[2]) + (fSpreadY * up[2]);
						vSpreadForward.NormalizeInPlace();

						Vector qaNewAngle;
						MATH::VectorAngles(vSpreadForward, qaNewAngle);
						qaNewAngle = MATH::NormalizeAngle(qaNewAngle);

						Vector vEnd;
						MATH::AngleVectors(qaNewAngle, &vEnd);
						vEnd = local_player->GetVecOrigin() + local_player->GetViewOffset() + (vEnd * 8192.f);

						SDK::trace_t trace;
						SDK::CTraceFilterOneEntity2 filter;
						filter.pEntity = entity;
						SDK::Ray_t ray;
						ray.Init(local_player->GetVecOrigin() + local_player->GetViewOffset(), vEnd);

						INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
						if (trace.m_pEnt == entity)
							iHit++;

						if (iHit / 256.f >= SETTINGS::ragebot_configs.aimbot_accuracy / 100.f)
							return true;
					}

					return false;
				}
				case 2: /// spread limit
				{
					auto weapon = UTILS::GetWeapon(local_player);
					if (!weapon)
						return false;

					return weapon->GetInnaccuracy() <= SETTINGS::ragebot_configs.aimbot_accuracy;
				}
			}

			return false;
		}

		void Aimbot::GetMultipointPositions(SDK::CBaseEntity* entity, std::vector<Vector>& positions, int hitbox_index, float pointscale)
		{
			const auto hitbox = GetHitbox(entity, hitbox_index);
			if (!hitbox)
				return;

			const float hitbox_radius = hitbox->m_flRadius * pointscale;

			// 2 spheres
			Vector Min = hitbox->bbmin, Max = hitbox->bbmax;

			if (hitbox->m_flRadius == -1.f)
			{
				const auto center = (Min + Max) * 0.5f;

				positions.emplace_back();
			}
			else
			{
				// Points 0 - 5 = min
				// Points 6 - 11 = max
				Vector P[12];
				for (int j = 0; j < 6; j++) { P[j] = Min; }
				for (int j = 7; j < 12; j++) { P[j] = Max; }

				P[1].x += hitbox_radius;
				P[2].x -= hitbox_radius;
				P[3].y += hitbox_radius;
				P[4].y -= hitbox_radius;
				P[5].z += hitbox_radius;

				P[6].x += hitbox_radius;
				P[7].x -= hitbox_radius;
				P[8].y += hitbox_radius;
				P[9].y -= hitbox_radius;
				P[10].z += hitbox_radius;
				P[11].z -= hitbox_radius;

				for (int j = 0; j < 12; j++)
				{
					MATH::VectorTransform(P[j], entity->GetBoneMatrix(hitbox->bone), P[j]);
					//Points[j] += OriginOffset;
					positions.push_back(P[j]);
				}
			}
		}

		Vector Aimbot::GetHitboxPosition(SDK::CBaseEntity* entity, int hitbox_index)
		{
			const auto hitbox = GetHitbox(entity, hitbox_index);
			if (!hitbox)
				return Vector(0, 0, 0);

			const auto bone_matrix = entity->GetBoneMatrix(hitbox->bone);

			Vector bbmin, bbmax;
			MATH::VectorTransform(hitbox->bbmin, bone_matrix, bbmin);
			MATH::VectorTransform(hitbox->bbmax, bone_matrix, bbmax);

			return (bbmin + bbmax) * 0.5f;
		}

		SDK::mstudiobbox_t* Aimbot::GetHitbox(SDK::CBaseEntity* entity, int hitbox_index)
		{
			if (entity->GetIsDormant() || entity->GetHealth() <= 0)
				return nullptr;

			const auto pModel = entity->GetModel();
			if (!pModel)
				return nullptr;

			auto pStudioHdr = INTERFACES::ModelInfo->GetStudioModel(pModel);
			if (!pStudioHdr)
				return nullptr;

			auto pSet = pStudioHdr->GetHitboxSet(0);
			if (!pSet)
				return nullptr;

			if (hitbox_index >= pSet->numhitboxes || hitbox_index < 0)
				return nullptr;

			return pSet->GetHitbox(hitbox_index);
		}
	}
}
