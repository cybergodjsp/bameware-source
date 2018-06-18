#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../UTILS/render.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/GameEvents.h"
#include "../SDK/CTrace.h"
#include "../SDK/Collideable.h"

#include "../FEATURES/Backtracking.h"
#include "../FEATURES/InGameLogger.h"
#include "../FEATURES/Autowall.h"

#include "Resolver.h"
#include "Configurations.h"
#include "Backtracking.h"

namespace FEATURES
{
	namespace RAGEBOT
	{
		Resolver resolver;

		Resolver::Resolver() { }

		void Resolver::DoFSN()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			for (int i = 0; i < 64; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam())
					continue;

				UpdateResolveRecord(entity);

				/// make sure to do dormant check AFTER calling UpdateResolveRecord()
				if (entity->GetIsDormant())
					continue;

				ResolveYaw(entity);
				ResolvePitch(entity);

				const auto resolve_record = player_resolve_records[i];
				entity->SetEyeAngles(resolve_record.resolved_angles);
			}
		}

		void Resolver::DoCM()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			ProcessSnapShots();

			last_eye_positions.insert(last_eye_positions.begin(), local_player->GetVecOrigin() + local_player->GetViewOffset());
			if (last_eye_positions.size() > 128)
				last_eye_positions.pop_back();

			auto nci = INTERFACES::Engine->GetNetChannelInfo();
			if (!nci)
				return;

			const int latency_ticks = TIME_TO_TICKS(nci->GetLatency(FLOW_OUTGOING));
			const auto latency_based_eye_pos = last_eye_positions.size() <= latency_ticks ? last_eye_positions.back() : last_eye_positions[latency_ticks];
			RENDER::render_queue.AddDrawCircle(latency_based_eye_pos, 2, 50, RED);
			for (int i = 0; i < 64; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam() || entity->GetIsDormant())
					continue;

				auto& resolve_record = player_resolve_records[i];

				const float height = 64;

				Vector direction_1, direction_2;
				MATH::AngleVectors(Vector(0.f, MATH::CalcAngle(local_player->GetVecOrigin(), entity->GetVecOrigin()).y - 90.f, 0.f), &direction_1);
				MATH::AngleVectors(Vector(0.f, MATH::CalcAngle(local_player->GetVecOrigin(), entity->GetVecOrigin()).y + 90.f, 0.f), &direction_2);

				const auto left_eye_pos = entity->GetVecOrigin() + Vector(0, 0, height) + (direction_1 * 16.f);
				const auto right_eye_pos = entity->GetVecOrigin() + Vector(0, 0, height) + (direction_2 * 16.f);

				resolve_record.anti_freestanding_record.left_damage = FEATURES::RAGEBOT::autowall.CalculateDamage(latency_based_eye_pos,
				                                                                                                  left_eye_pos, local_player, entity, 1).damage;
				resolve_record.anti_freestanding_record.right_damage = FEATURES::RAGEBOT::autowall.CalculateDamage(latency_based_eye_pos,
				                                                                                                   right_eye_pos, local_player, entity, 1).damage;

				SDK::Ray_t ray;
				SDK::trace_t trace;
				SDK::CTraceWorldOnly filter;

				ray.Init(left_eye_pos, latency_based_eye_pos);
				INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
				resolve_record.anti_freestanding_record.left_fraction = trace.flFraction;

				ray.Init(right_eye_pos, latency_based_eye_pos);
				INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
				resolve_record.anti_freestanding_record.right_fraction = trace.flFraction;

				resolve_record.did_predicted_lby_flick = false;
				resolve_record.did_lby_flick = false;
			}
		}

		void Resolver::AddShotSnapshot(SDK::CBaseEntity* entity, PlayerResolveRecord resolve_record)
		{
			ShotSnapshot snapshot;

			snapshot.entity = entity;
			snapshot.time = UTILS::GetCurtime();
			snapshot.resolve_record = resolve_record;
			snapshot.first_processed_time = 0.f;
			snapshot.was_shot_processed = false;
			snapshot.hitgroup_hit = -1;

			shot_snapshots.push_back(snapshot);
		}

		void Resolver::EventCallback(SDK::IGameEvent* game_event)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0 || !game_event)
				return;

			if (shot_snapshots.size() <= 0)
				return;

			auto& snapshot = shot_snapshots.front();
			switch (UTILS::FNVHashEx(game_event->GetName()))
			{
				case UTILS::FNVHashEx("player_hurt"):
				{
					/// it wasn't us who shot
					if (INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt(enc_char("attacker"))) != INTERFACES::Engine->GetLocalPlayer())
						return;

					if (!snapshot.was_shot_processed)
					{
						snapshot.was_shot_processed = true;
						snapshot.first_processed_time = UTILS::GetCurtime();
					}

					snapshot.hitgroup_hit = game_event->GetInt(enc_char("hitgroup"));

					break;
				}
				case UTILS::FNVHashEx("bullet_impact"):
				case UTILS::FNVHashEx("weapon_fire"):
				{
					/// it wasn't us who shot
					if (INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt(enc_char("userid"))) != INTERFACES::Engine->GetLocalPlayer())
						return;

					if (!snapshot.was_shot_processed)
					{
						snapshot.was_shot_processed = true;
						snapshot.first_processed_time = UTILS::GetCurtime();
					}

					break;
				}
			}
		}

		void Resolver::ProcessSnapShots()
		{
			if (shot_snapshots.size() <= 0)
				return;

			const auto snapshot = shot_snapshots.front();
			if (fabs(UTILS::GetCurtime() - snapshot.time) > 1.f)
			{
				LOG(enc_char("** Unhandled shot snapshot!"));
				FEATURES::MISC::in_game_logger.AddLog(FEATURES::MISC::InGameLogger::Log(enc_char("Unhandled shot snapshot!"), RED));

				shot_snapshots.erase(shot_snapshots.begin());
				return;
			}

			const int player_index = snapshot.entity->GetIndex();
			if (snapshot.hitgroup_hit != -1) /// hit
			{
				/// increment shots fired and shots hit
				for (int i = 0; i < RESOLVE_TYPE_NUM; i++)
				{
					if (snapshot.resolve_record.resolve_type & (1 << i))
					{
						player_resolve_records[player_index].shots_fired[i]++;
						player_resolve_records[player_index].shots_hit[i]++;
					}
				}

				FEATURES::MISC::in_game_logger.AddLog(FEATURES::MISC::InGameLogger::Log("Hit shot at " + TranslateResolveRecord(snapshot.resolve_record.resolve_type), GREEN));
			}
			else if (snapshot.first_processed_time != 0.f && fabs(UTILS::GetCurtime() - snapshot.first_processed_time) > 0.1f) /// missed
			{
				/// increment shots fired but not shots hit
				for (int i = 0; i < RESOLVE_TYPE_NUM; i++)
				{
					if (snapshot.resolve_record.resolve_type & (1 << i))
						player_resolve_records[player_index].shots_fired[i]++;
				}

				if (snapshot.resolve_record.resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY)
					player_resolve_records[player_index].shots_missed_moving_lby++;

				if (snapshot.resolve_record.resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY_DELTA)
					player_resolve_records[player_index].shots_missed_moving_lby_delta++;

				FEATURES::MISC::in_game_logger.AddLog(FEATURES::MISC::InGameLogger::Log("Missed shot at " + TranslateResolveRecord(snapshot.resolve_record.resolve_type), RED));
			}
			else
				return;

			shot_snapshots.erase(shot_snapshots.begin());
		}

		void Resolver::UpdateResolveRecord(SDK::CBaseEntity* entity)
		{
			/// a copy of the previous record for comparisons and shit
			const auto previous_record = player_resolve_records[entity->GetIndex()];
			auto& record = player_resolve_records[entity->GetIndex()];

			record.resolved_angles = record.networked_angles;
			record.velocity = entity->GetVelocity();
			record.origin = entity->GetVecOrigin();
			record.lower_body_yaw = entity->GetLowerBodyYaw();
			record.is_dormant = entity->GetIsDormant();
			record.has_fake = IsFakingYaw(entity);
			record.resolve_type = 0;

			record.is_balance_adjust_triggered = false , record.is_balance_adjust_playing = false;
			for (int i = 0; i < 15; i++) /// go through each animation layer
			{
				record.anim_layers[i] = entity->GetAnimOverlay(i);

				/// balanceadjust act
				if (entity->GetSequenceActivity(record.anim_layers[i].m_nSequence) == SDK::CSGO_ACTS::ACT_CSGO_IDLE_TURN_BALANCEADJUST)
				{
					record.is_balance_adjust_playing = true;

					/// balance adjust is being triggered
					if (record.anim_layers[i].m_flWeight == 1 || record.anim_layers[i].m_flCycle > previous_record.anim_layers[i].m_flCycle)
						record.last_balance_adjust_trigger_time = UTILS::GetCurtime();
					if (fabs(UTILS::GetCurtime() - record.last_balance_adjust_trigger_time) < 0.5f)
						record.is_balance_adjust_triggered = true;
				}
			}

			if (record.is_dormant)
				record.next_predicted_lby_update = FLT_MAX;

			/// if lby updated
			if (record.lower_body_yaw != previous_record.lower_body_yaw && !record.is_dormant && !previous_record.is_dormant)
				record.did_lby_flick = true;

			/// fakewalk
			const bool is_moving_on_ground = record.velocity.Length2D() > 50 && entity->GetFlags() & FL_ONGROUND;
			if (is_moving_on_ground && record.is_balance_adjust_triggered)
				record.is_fakewalking = true;
			else
				record.is_fakewalking = false;

			/// last moving lby, using a high velocity check ***just in case*** of fakewalkers
			if (is_moving_on_ground && !record.is_fakewalking && record.velocity.Length2D() > 1.f && !record.is_dormant)
			{
				record.is_last_moving_lby_valid = true;
				record.is_last_moving_lby_delta_valid = false;
				record.shots_missed_moving_lby = 0;
				record.shots_missed_moving_lby_delta = 0;
				record.last_moving_lby = record.lower_body_yaw + 45;
				record.last_time_moving = UTILS::GetCurtime();
			}

			/// just came out of dormancy
			if (!record.is_dormant && previous_record.is_dormant)
			{
				/// if moved more than 32 units
				if ((record.origin - previous_record.origin).Length2D() > 16.f)
					record.is_last_moving_lby_valid = false;
			}

			/// get last moving lby delta
			if (!record.is_last_moving_lby_delta_valid && record.is_last_moving_lby_valid && record.velocity.Length2D() < 20 && fabs(UTILS::GetCurtime() - record.last_time_moving) < 1.0)
			{
				/// if lby updated
				if (record.lower_body_yaw != previous_record.lower_body_yaw)
				{
					record.last_moving_lby_delta = MATH::NormalizeYaw(record.last_moving_lby - record.lower_body_yaw);
					record.is_last_moving_lby_delta_valid = true;
				}
			}

			if (MATH::NormalizePitch(record.networked_angles.x) > 5.f)
				record.last_time_down_pitch = UTILS::GetCurtime();
		}

		void Resolver::ResolveYaw(SDK::CBaseEntity* entity)
		{
			auto& resolve_record = player_resolve_records[entity->GetIndex()];

			if (resolve_record.did_lby_flick)
			{
				resolve_record.resolved_angles.y = resolve_record.lower_body_yaw;
				resolve_record.resolve_type |= RESOLVE_TYPE_LBY_UPDATE;
			}
			else if (resolve_record.did_predicted_lby_flick)
			{
				resolve_record.resolved_angles.y = resolve_record.lower_body_yaw;
				resolve_record.resolve_type |= RESOLVE_TYPE_PREDICTED_LBY_UPDATE;
			}
			else if (const auto bt_recs = FEATURES::RAGEBOT::backtracking.GetRecords(entity); bt_recs.size() >= 1 && !bt_recs.front().resolve_record.has_fake && !IsFakingYaw(entity))
			{
				resolve_record.resolved_angles.y = resolve_record.networked_angles.y;
				resolve_record.resolve_type |= RESOLVE_TYPE_NO_FAKE;
			}
			else if (const float override_yaw = ResolveYawOverride(entity); override_yaw != FLT_MAX)
			{
				resolve_record.resolved_angles.y = override_yaw;
				resolve_record.resolve_type |= RESOLVE_TYPE_OVERRIDE;
			}
			else if (IsMovingOnGround(entity) && !IsFakewalking(entity)) /// moving
			{
				resolve_record.resolved_angles.y = resolve_record.last_moving_lby;
				resolve_record.resolve_type |= RESOLVE_TYPE_LBY;
			}
			else if (resolve_record.is_last_moving_lby_valid && resolve_record.shots_missed_moving_lby < 1 && IsYawSideways(entity, resolve_record.last_moving_lby)) /// last moving lby if sideways prioritizes over antiurine
			{
				resolve_record.resolved_angles.y = resolve_record.last_moving_lby;
				resolve_record.resolve_type |= RESOLVE_TYPE_LAST_MOVING_LBY;
			}
			else if (AntiFreestanding(entity, resolve_record.resolved_angles.y))
			{
				resolve_record.resolve_type |= RESOLVE_TYPE_ANTI_FREESTANDING;
			}
			else if (resolve_record.is_last_moving_lby_valid && resolve_record.shots_missed_moving_lby < 1) /// last moving lby
			{
				resolve_record.resolved_angles.y = resolve_record.last_moving_lby;
				resolve_record.resolve_type |= RESOLVE_TYPE_LAST_MOVING_LBY;
			}
			else /// bruteforce as last resort
				ResolveYawBruteforce(entity);
		}

		void Resolver::ResolvePitch(SDK::CBaseEntity* entity)
		{
			auto& resolve_record = player_resolve_records[entity->GetIndex()];

			if (resolve_record.resolve_type & RESOLVE_TYPE_LBY_UPDATE || resolve_record.resolve_type & RESOLVE_TYPE_PREDICTED_LBY_UPDATE ||
				resolve_record.resolve_type & RESOLVE_TYPE_NO_FAKE)
				resolve_record.resolved_angles.x = resolve_record.networked_angles.x;
			else
			{
				/// super fucking ghetto fix to stop their pitch from going to 0 when they're shooting (only neccesary when they're using psilent)
				if (fabs(UTILS::GetCurtime() - resolve_record.last_time_down_pitch) < 0.5f)
					resolve_record.resolved_angles.x = 70.f;
				else
					resolve_record.resolved_angles.x = resolve_record.networked_angles.x;
			}

			resolve_record.resolved_angles.x = MATH::NormalizePitch(resolve_record.resolved_angles.x);
		}

		void Resolver::ResolveYawBruteforce(SDK::CBaseEntity* entity)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			auto& resolve_record = player_resolve_records[entity->GetIndex()];
			resolve_record.resolve_type |= RESOLVE_TYPE_BRUTEFORCE;

			const float at_target_yaw = MATH::CalcAngle(entity->GetVecOrigin(), local_player->GetVecOrigin()).y;

			const int shots_missed = resolve_record.shots_fired[GetResolveTypeIndex(resolve_record.resolve_type)] -
				resolve_record.shots_hit[GetResolveTypeIndex(resolve_record.resolve_type)];
			switch (shots_missed % 3)
			{
				case 0:
					resolve_record.resolved_angles.y = UTILS::GetLBYRotatedYaw(entity->GetLowerBodyYaw(), at_target_yaw + 60.f);
					break;
				case 1:
					resolve_record.resolved_angles.y = at_target_yaw + 140.f;
					break;
				case 2:
					resolve_record.resolved_angles.y = at_target_yaw - 75.f;
					break;
			}
		}

		float Resolver::ResolveYawOverride(SDK::CBaseEntity* entity)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return FLT_MAX;

			if (SETTINGS::main_configs.override_keybind == -1 || !GetAsyncKeyState(SETTINGS::main_configs.override_keybind))
				return FLT_MAX;

			Vector viewangles;
			INTERFACES::Engine->GetViewAngles(viewangles);

			const float at_target_yaw = MATH::CalcAngle(local_player->GetVecOrigin(), entity->GetVecOrigin()).y;
			if (fabs(MATH::NormalizeYaw(viewangles.y - at_target_yaw)) > 10.f)
				return FLT_MAX;

			return UTILS::GetLBYRotatedYaw(entity->GetLowerBodyYaw(), (MATH::NormalizeYaw(viewangles.y - at_target_yaw) > 0) ? at_target_yaw + 90.f : at_target_yaw - 90.f);
		}

		bool Resolver::AntiFreestanding(SDK::CBaseEntity* entity, float& yaw)
		{
			const auto freestanding_record = player_resolve_records[entity->GetIndex()].anti_freestanding_record;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			if (freestanding_record.left_damage >= 20 && freestanding_record.right_damage >= 20)
				return false;

			const float at_target_yaw = MATH::CalcAngle(local_player->GetVecOrigin(), entity->GetVecOrigin()).y;
			if (freestanding_record.left_damage <= 0 && freestanding_record.right_damage <= 0)
			{
				if (freestanding_record.right_fraction < freestanding_record.left_fraction)
					yaw = at_target_yaw + 125.f;
				else
					yaw = at_target_yaw - 73.f;
			}
			else
			{
				if (freestanding_record.left_damage > freestanding_record.right_damage)
					yaw = at_target_yaw + 130.f;
				else
					yaw = at_target_yaw - 49.f;
			}

			return true;
		}

		bool Resolver::IsYawSideways(SDK::CBaseEntity* entity, float yaw)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
					return false;
		
				const auto at_target_yaw = MATH::CalcAngle(local_player->GetVecOrigin(), entity->GetVecOrigin()).y;
			const float delta = fabs(MATH::NormalizeYaw(at_target_yaw - yaw));
		
				return delta > 20.f && delta < 160.f;
		}

		bool Resolver::IsFakingYaw(SDK::CBaseEntity* entity)
		{
			const auto bt_records = FEATURES::RAGEBOT::backtracking.GetRecords(entity);
			if (bt_records.size() < 2)
				return true;

			if (bt_records.front().is_exploiting)
				return true;

			if (fabs(bt_records[0].simulation_time - bt_records[1].simulation_time) == INTERFACES::Globals->interval_per_tick)
				return false;

			return true;
		}

		std::string Resolver::TranslateResolveRecord(unsigned short resolve_type)
		{
			std::vector<std::string> resolve_strings;

			/// gonna have to use lorge if statements cuz fuck you
			if (resolve_type & RESOLVE_TYPE_OVERRIDE)
				resolve_strings.push_back(enc_str("OVERRIDE"));
			if (resolve_type & RESOLVE_TYPE_NO_FAKE)
				resolve_strings.push_back(enc_str("NO FAKE"));
			if (resolve_type & RESOLVE_TYPE_LBY)
				resolve_strings.push_back(enc_str("LBY"));
			if (resolve_type & RESOLVE_TYPE_LBY_UPDATE)
				resolve_strings.push_back(enc_str("LBY UPDATE"));
			if (resolve_type & RESOLVE_TYPE_PREDICTED_LBY_UPDATE)
				resolve_strings.push_back(enc_str("PREDICTED LBY UPDATE"));
			if (resolve_type & RESOLVE_TYPE_NOT_BREAKING_LBY)
				resolve_strings.push_back(enc_str("NOT BREAKING LBY"));
			if (resolve_type & RESOLVE_TYPE_BRUTEFORCE)
				resolve_strings.push_back(enc_str("BRUTEFORCE"));
			if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY_DELTA)
				resolve_strings.push_back(enc_str("LAST MOVING LBY DELTA"));
			if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY)
				resolve_strings.push_back(enc_str("LAST MOVING LBY"));
			if (resolve_type & RESOLVE_TYPE_ANTI_FREESTANDING)
				resolve_strings.push_back(enc_str("ANTI FREESTAND"));

			if (resolve_strings.size() <= 0)
				return "GAY";

			std::string buffer;
			for (const auto& str : resolve_strings)
				buffer.size() ? (buffer += " / " + str) : (buffer = str);

			return buffer;
		}

		CColor Resolver::GetResolveColor(unsigned short resolve_type)
		{
			/// gonna have to use lorge if statements cuz fuck you
			if (resolve_type & RESOLVE_TYPE_OVERRIDE)
				return RED;
			else if (resolve_type & RESOLVE_TYPE_NO_FAKE)
				return GREEN;
			else if (resolve_type & RESOLVE_TYPE_LBY)
				return GREEN;
			else if (resolve_type & RESOLVE_TYPE_LBY_UPDATE)
				return GREEN;
			else if (resolve_type & RESOLVE_TYPE_PREDICTED_LBY_UPDATE)
				return GREEN;

			else if (resolve_type & RESOLVE_TYPE_NOT_BREAKING_LBY)
				return ORANGE;
			else if (resolve_type & RESOLVE_TYPE_BRUTEFORCE)
				return RED;
			else if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY_DELTA)
				return ORANGE;
			else if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY)
				return ORANGE;
			else if (resolve_type & RESOLVE_TYPE_ANTI_FREESTANDING)
				return ORANGE;

			return WHITE;
		}

		int Resolver::GetResolveTypeIndex(unsigned short resolve_type)
		{
			/// gonna have to use lorge if statements cuz fuck you
			if (resolve_type & RESOLVE_TYPE_OVERRIDE)
				return 0;
			else if (resolve_type & RESOLVE_TYPE_NO_FAKE)
				return 1;
			else if (resolve_type & RESOLVE_TYPE_LBY)
				return 2;
			else if (resolve_type & RESOLVE_TYPE_LBY_UPDATE)
				return 3;
			else if (resolve_type & RESOLVE_TYPE_PREDICTED_LBY_UPDATE)
				return 4;
			else if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY)
				return 5;
			else if (resolve_type & RESOLVE_TYPE_NOT_BREAKING_LBY)
				return 6;
			else if (resolve_type & RESOLVE_TYPE_BRUTEFORCE)
				return 7;
			else if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY_DELTA)
				return 8;
			else if (resolve_type & RESOLVE_TYPE_ANTI_FREESTANDING)
				return 9;

			return 0;
		}

		int Resolver::GetShotsMissed(SDK::CBaseEntity* entity, unsigned short resolve_type)
		{
			const int resolve_index = GetResolveTypeIndex(resolve_type);

			return player_resolve_records[entity->GetIndex()].shots_fired[resolve_index] - player_resolve_records[entity->GetIndex()].shots_hit[resolve_index];
		}
	}
}
