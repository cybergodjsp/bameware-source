#include "../includes.h"

#include "../UTILS/variables.h"
#include "../UTILS/variables.h"
#include "../UTILS/interfaces.h"
#include "../UTILS/render.h"

#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/NetChannel.h"
#include "../SDK/CBaseWeapon.h"

#include "Configurations.h"
#include "Autowall.h"

#include "AntiAim.h"
#include "Aimbot.h"

namespace FEATURES
{
	namespace RAGEBOT
	{
		Antiaim antiaim;

		void Antiaim::Do(SDK::CUserCmd* cmd)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0 || local_player->GetMovetype() == MOVETYPE_NOCLIP || local_player->GetMovetype() == MOVETYPE_LADDER || !SETTINGS::ragebot_configs.antiaim_enabled)
				return;

			auto weapon = UTILS::GetWeapon(local_player);
			if (!weapon || UTILS::IsWeaponGrenade(weapon) && UTILS::CanShoot())
				return;

			auto IsPickingUpWeapon = [local_player]() -> bool
				{
					Vector viewangles;
					INTERFACES::Engine->GetViewAngles(viewangles);

					Vector direction;
					MATH::AngleVectors(viewangles, &direction);

					const auto start = local_player->GetVecOrigin() + local_player->GetViewOffset();
					const auto end = start + (direction * 130.f);

					SDK::Ray_t ray;
					ray.Init(start, end);

					SDK::trace_t trace;
					SDK::CTraceFilter filter;
					filter.pSkip1 = local_player;

					INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
					if (!trace.m_pEnt)
						return false;

					if (const auto client_class = trace.m_pEnt->GetClientClass();
					!client_class || !trace.m_pEnt->IsWeapon()
					)
					return false;

					return true;
				};

			if ((cmd->buttons & IN_USE && IsPickingUpWeapon()) || (cmd->buttons & IN_ATTACK && UTILS::CanShoot()) || (cmd->buttons & IN_ATTACK2 && UTILS::CanShoot(true)))
				return;

			cmd->viewangles.x = DoPitch();
			cmd->viewangles.y = DoYaw();

			cmd->viewangles = MATH::NormalizeAngle(cmd->viewangles);
		}

		void Antiaim::UpdateLBY()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || !(local_player->GetFlags() & FL_ONGROUND))
				return;

			auto animstate = local_player->GetAnimState();
			if (!animstate)
				return;

			auto net_channel = INTERFACES::Engine->GetNetChannel();
			if (!net_channel || net_channel->m_nChokedPackets)
				return;

			const float curtime = UTILS::GetCurtime();
			if (animstate->speed_2d > 0.1 && !GLOBAL::is_fakewalking)
			{
				m_next_lby_update_time = curtime + 0.22;
				m_last_move_time = curtime;
			}
			if (static float last_lby;
			last_lby != local_player->GetLowerBodyYaw()
			)
			{
				last_lby = local_player->GetLowerBodyYaw();
				if (fabs(curtime - m_last_move_time) > 0.3 && fabs(MATH::NormalizeYaw(local_player->GetLowerBodyYaw() - m_last_attempted_lby)) > 35.f)
					m_next_lby_update_time = curtime + 1.1 + INTERFACES::Globals->interval_per_tick;
			}

			if (m_next_lby_update_time < curtime)
			{
				m_next_lby_update_time = curtime + 1.1;
				m_will_lby_update = true;
			}
		}

		float Antiaim::BreakLBY(float real_yaw, float desired_lby, bool double_flick)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return real_yaw;

			desired_lby = MATH::GetYawDelta(desired_lby, local_player->GetLowerBodyYaw()) < 35.f ? local_player->GetLowerBodyYaw() : desired_lby;
			if (m_will_lby_update)
			{
				static bool switch_bool = false;
				switch_bool = !switch_bool;
				if (const float lby_delta = MATH::NormalizeYaw(desired_lby - real_yaw);
				fabs(lby_delta) < 100.f
				)
				desired_lby = real_yaw + (switch_bool ? lby_delta : -lby_delta);

				m_will_lby_update = false;
				m_last_attempted_lby = desired_lby;
				return desired_lby;
			}

			if (fabs(MATH::NormalizeYaw(real_yaw - desired_lby)) > 130.f && fabs(MATH::NormalizeYaw(desired_lby - local_player->GetLowerBodyYaw())) < 35.f)
			{
				if (fabs(UTILS::GetCurtime() - m_next_lby_update_time) < INTERFACES::Globals->interval_per_tick)
					return desired_lby - 115.f;
				if (fabs(UTILS::GetCurtime() - m_next_lby_update_time) > 1.1 - (INTERFACES::Globals->interval_per_tick * 5))
					return desired_lby + 115.f;
			}

			return real_yaw;
		}

		float Antiaim::DoYaw()
		{
			using namespace SETTINGS;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return 0.f;

			bool is_jumping = !(local_player->GetFlags() & FL_ONGROUND);
			bool is_moving = (local_player->GetVelocity().Length2D() > 0.1f) && !is_jumping && !GLOBAL::is_fakewalking;

			/// returns viewangles/at target viewangles
			auto GetViewangles = [local_player](bool at_target_enabled) -> float
				{
					Vector viewangles;
					INTERFACES::Engine->GetViewAngles(viewangles);

					if (!at_target_enabled)
						return viewangles.y;

					/// get at target based on closest fov
					float closest_fov = 360.f, closest_fov_yaw = 181.f;
					for (int i = 0; i < 64; i++)
					{
						auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
						if (!entity || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam() || entity->GetIsDormant())
							continue;

						const float current_yaw = MATH::CalcAngle(local_player->GetVecOrigin(), entity->GetVecOrigin()).y;
						const float current_fov = fabs(MATH::NormalizeYaw(current_yaw - viewangles.y));
						if (current_fov < closest_fov)
						{
							closest_fov = current_fov;
							closest_fov_yaw = current_yaw;
						}
					}

					/// couldn't find a valid entity
					if (closest_fov == 360.f)
						return viewangles.y;

					return closest_fov_yaw;
				};

			/// returns the base real angle from the combobox setting
			auto GetBaseRealAngle = [local_player, GetViewangles](int setting, bool at_target_enabled, float spin_speed) -> float
				{
					/// base yaw
					switch (setting)
					{
						case 0: /// viewangle
							return GetViewangles(at_target_enabled);
						case 1: /// static
							return 0.f;
						case 2: /// lby
							return local_player->GetLowerBodyYaw();
						case 3: /// adaptive
							return GetViewangles(at_target_enabled);
						case 4: /// spin
							return UTILS::GetCurtime() * spin_speed * 100.f;
					}

					return 0.f;
				};

			/// returns the base fake angle from the combobox setting
			auto GetBaseFakeAngle = [local_player, GetViewangles](int setting, bool at_target_enabled, float spin_speed) -> float
				{
					/// base yaw
					switch (setting)
					{
						case 0: /// viewangle
							return GetViewangles(at_target_enabled);
						case 1: /// static
							return 0.f;
						case 2: /// lby
							return local_player->GetLowerBodyYaw();
						case 3: /// adaptive
							return GetViewangles(at_target_enabled);
						case 4: /// spin
							return UTILS::GetCurtime() * spin_speed * 100.f;
						case 5: /// real inverse
							return GLOBAL::real_angles.y + 180.f;
					}

					return 0.f;
				};

			auto DoRealYaw = [this, is_jumping, is_moving, local_player, GetViewangles, GetBaseRealAngle]() -> float
				{
					float real_yaw = GetViewangles(false), lby = 0.f;

					if (is_jumping)
					{
						real_yaw = GetBaseRealAngle(ragebot_configs.antiaim_jumping_real_yaw,
						                            ragebot_configs.antiaim_jumping_at_target_enabled, ragebot_configs.antiaim_jumping_real_spin_speed);

						/// offset
						if (ragebot_configs.antiaim_jumping_real_yaw == 3) /// adaptive
							real_yaw += MATH::NormalizeYaw(GetViewangles(false) - GetViewangles(true)) > 0.f ? ragebot_configs.antiaim_jumping_real_offset : -ragebot_configs.antiaim_jumping_real_offset;
						else
							real_yaw += ragebot_configs.antiaim_jumping_real_offset;

						/// jitter range
						real_yaw += MATH::RandomNumber(ragebot_configs.antiaim_jumping_real_jrange * -0.5f, ragebot_configs.antiaim_jumping_real_jrange * 0.5f);
					}
					else if (is_moving)
					{
						real_yaw = GetBaseRealAngle(ragebot_configs.antiaim_moving_real_yaw,
						                            ragebot_configs.antiaim_moving_at_target_enabled, ragebot_configs.antiaim_moving_real_spin_speed);

						/// offset
						if (ragebot_configs.antiaim_moving_real_yaw == 3) /// adaptive
							real_yaw += MATH::NormalizeYaw(GetViewangles(false) - GetViewangles(true)) > 0.f ? ragebot_configs.antiaim_moving_real_offset : -ragebot_configs.antiaim_moving_real_offset;
						else
							real_yaw += ragebot_configs.antiaim_moving_real_offset;

						/// jitter range
						real_yaw += MATH::RandomNumber(ragebot_configs.antiaim_moving_real_jrange * -0.5f, ragebot_configs.antiaim_moving_real_jrange * 0.5f);
					}
					else /// standing
					{
						real_yaw = GetBaseRealAngle(ragebot_configs.antiaim_standing_real_yaw,
						                            ragebot_configs.antiaim_standing_at_target_enabled, ragebot_configs.antiaim_standing_real_spin_speed);

						/// offset
						if (ragebot_configs.antiaim_standing_real_yaw == 3) /// adaptive
							real_yaw += MATH::NormalizeYaw(GetViewangles(false) - GetViewangles(true)) > 0.f ? ragebot_configs.antiaim_standing_real_offset : -ragebot_configs.antiaim_standing_real_offset;
						else
							real_yaw += ragebot_configs.antiaim_standing_real_offset;

						/// make sure to do this before we add the jitter shit
						lby = real_yaw + SETTINGS::ragebot_configs.antiaim_standing_real_lby_delta;

						/// jitter range
						real_yaw += MATH::RandomNumber(ragebot_configs.antiaim_standing_real_jrange * -0.5f, ragebot_configs.antiaim_standing_real_jrange * 0.5f);
					}

					if (DoEdging(real_yaw, lby))
						real_yaw += MATH::RandomNumber(ragebot_configs.antiaim_edging_jrange * -0.5f, ragebot_configs.antiaim_edging_jrange * 0.5f);

					return BreakLBY(real_yaw, lby);
				};
			auto DoFakeYaw = [this, is_jumping, is_moving, local_player, GetViewangles, GetBaseFakeAngle]() -> float
				{
					float fake_yaw = GetViewangles(false);

					if (is_jumping)
					{
						fake_yaw = GetBaseFakeAngle(ragebot_configs.antiaim_jumping_fake_yaw,
						                            ragebot_configs.antiaim_jumping_at_target_enabled, ragebot_configs.antiaim_jumping_fake_spin_speed);

						/// offset
						if (ragebot_configs.antiaim_jumping_fake_yaw == 3) /// adaptive
							fake_yaw += MATH::NormalizeYaw(GetViewangles(false) - GetViewangles(true)) > 0.f ? ragebot_configs.antiaim_jumping_fake_offset : -ragebot_configs.antiaim_jumping_fake_offset;
						else
							fake_yaw += ragebot_configs.antiaim_jumping_fake_offset;

						/// jitter range
						fake_yaw += MATH::RandomNumber(ragebot_configs.antiaim_jumping_fake_jrange * -0.5f, ragebot_configs.antiaim_jumping_fake_jrange * 0.5f);
					}
					else if (is_moving)
					{
						fake_yaw = GetBaseFakeAngle(ragebot_configs.antiaim_moving_fake_yaw,
						                            ragebot_configs.antiaim_moving_at_target_enabled, ragebot_configs.antiaim_moving_fake_spin_speed);

						/// offset
						if (ragebot_configs.antiaim_moving_fake_yaw == 3) /// adaptive
							fake_yaw += MATH::NormalizeYaw(GetViewangles(false) - GetViewangles(true)) > 0.f ? ragebot_configs.antiaim_moving_fake_offset : -ragebot_configs.antiaim_moving_fake_offset;
						else
							fake_yaw += ragebot_configs.antiaim_moving_fake_offset;

						/// jitter range
						fake_yaw += MATH::RandomNumber(ragebot_configs.antiaim_moving_fake_jrange * -0.5f, ragebot_configs.antiaim_moving_fake_jrange * 0.5f);
					}
					else /// standing
					{
						fake_yaw = GetBaseFakeAngle(ragebot_configs.antiaim_standing_fake_yaw,
						                            ragebot_configs.antiaim_standing_at_target_enabled, ragebot_configs.antiaim_standing_fake_spin_speed);

						/// offset
						if (ragebot_configs.antiaim_standing_fake_yaw == 3) /// adaptive
							fake_yaw += MATH::NormalizeYaw(GetViewangles(false) - GetViewangles(true)) > 0.f ? ragebot_configs.antiaim_standing_fake_offset : -ragebot_configs.antiaim_standing_fake_offset;
						else
							fake_yaw += ragebot_configs.antiaim_standing_fake_offset;

						/// jitter range
						fake_yaw += MATH::RandomNumber(ragebot_configs.antiaim_standing_fake_jrange * -0.5f, ragebot_configs.antiaim_standing_fake_jrange * 0.5f);
					}

					float tmp_float;
					if (DoEdging(fake_yaw, tmp_float))
						fake_yaw += 180.f + MATH::RandomNumber(-75.f, 75.f);

					return fake_yaw;
				};

			return GLOBAL::should_send_packet ? DoFakeYaw() : DoRealYaw();
		}

		float Antiaim::DoPitch()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return 0.f;

			auto GetPitchFromSetting = [](int setting) -> float
				{
					Vector viewangles;
					INTERFACES::Engine->GetViewAngles(viewangles);

					switch (setting)
					{
						case 0:
							return viewangles.x;
						case 1:
							return 89.f;
						case 2:
							return -89.f;
						default:
							return MATH::RandomNumber(-89.f, 89.f);
					}
				};

			bool is_jumping = !(local_player->GetFlags() & FL_ONGROUND);
			bool is_moving = (local_player->GetVelocity().Length2D() > 0.1f) && !is_jumping;

			if (is_jumping)
				return GetPitchFromSetting(SETTINGS::ragebot_configs.antiaim_jumping_pitch);
			else if (is_moving)
				return GetPitchFromSetting(SETTINGS::ragebot_configs.antiaim_moving_pitch);
			else /// standing pitch
				return GetPitchFromSetting(SETTINGS::ragebot_configs.antiaim_standing_pitch);
		}

		bool Antiaim::DoEdging(float& yaw, float& lby)
		{
			using namespace SETTINGS;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			/// check if enabled
			const bool is_jumping = !(local_player->GetFlags() & FL_ONGROUND);
			const bool is_moving = (local_player->GetVelocity().Length2D() > 0.1) && !is_jumping;
			if (is_jumping)
			{
				if (!ragebot_configs.antiaim_edging_jumping_enabled)
					return false;
			}
			else if (is_moving)
			{
				if (!ragebot_configs.antiaim_edging_moving_enabled)
					return false;
			}
			else if (!ragebot_configs.antiaim_edging_standing_enabled)
				return false;

			Vector viewangles;
			INTERFACES::Engine->GetViewAngles(viewangles);

			auto DoWallDetection = [local_player, viewangles](float& yaw, float& lby) -> bool
				{
					SDK::trace_t trace;
					SDK::Ray_t ray;
					SDK::CTraceWorldOnly filter;

					static constexpr float trace_distance = 25.f;
					const auto head_position = local_player->GetVecOrigin() + local_player->GetViewOffset();

					float last_fraction = 1.f;
					for (int i = 0; i < 360; i += 2)
					{
						Vector direction;
						MATH::AngleVectors(Vector(0, i, 0), &direction);

						ray.Init(head_position, head_position + (direction * trace_distance));
						INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

						if (trace.flFraction > last_fraction)
						{
							yaw = i - 2;
							lby = yaw + SETTINGS::ragebot_configs.antiaim_edging_lby_delta;

							return true;
						}

						last_fraction = trace.flFraction;
					}

					return false;
				};
			auto DoFreestanding = [local_player, viewangles, is_moving, this](float& yaw, float& lby) -> bool
				{
					static constexpr int damage_tolerance = 10;

					std::vector<SDK::CBaseEntity*> enemies;

					/// Find the lowest fov enemy
					SDK::CBaseEntity* closest_enemy = nullptr;
					float lowest_fov = 360.f;
					for (int i = 0; i < 64; i++)
					{
						auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
						if (!entity || entity->GetIsDormant() || entity->GetTeam() == local_player->GetTeam() || entity->GetHealth() <= 0)
							continue;

						const float current_fov = fabs(MATH::NormalizeYaw(MATH::CalcAngle(local_player->GetVecOrigin(), entity->GetVecOrigin()).y - viewangles.y));
						if (current_fov < lowest_fov)
						{
							lowest_fov = current_fov;
							closest_enemy = entity;
						}

						enemies.push_back(entity);
					}

					/// No enemies
					if (closest_enemy == nullptr)
						return false;

					const float at_target_yaw = MATH::CalcAngle(local_player->GetVecOrigin(), closest_enemy->GetVecOrigin()).y;
					const float right_yaw = at_target_yaw - 90.f;
					const float left_yaw = at_target_yaw + 90.f;

					/// Misc functions
					auto CalcDamage = [local_player, enemies](Vector point) -> int
						{
							int damage = 0;
							for (auto& enemy : enemies)
							{
								damage += UTILS::Max<int>(FEATURES::RAGEBOT::autowall.CalculateDamage(enemy->GetVecOrigin() + Vector(0, 0, 64.f), point, enemy, local_player, 1).damage,
								                          FEATURES::RAGEBOT::autowall.CalculateDamage(enemy->GetVecOrigin() + Vector(0, 0, 49.f), point, enemy, local_player, 1).damage);
							}

							return damage;
						};
					auto RotateAndExtendPosition = [](Vector position, float yaw, float distance) -> Vector
						{
							Vector direction;
							MATH::AngleVectors(Vector(0, yaw, 0), &direction);

							return position + (direction * distance);
						};
					auto RotateLBYAndYaw = [right_yaw, left_yaw, local_player](int right_damage, int left_damage, float lby_delta, float& yaw, float& lby, bool prefect_angle = false, bool symetrical = true) -> bool
						{
							const bool prefer_right = right_damage < left_damage;

							yaw = prefer_right ? right_yaw : left_yaw;
							if (symetrical)
								lby_delta = lby_delta * (prefer_right ? -1.f : 1.f);

							/// If not moving
							if (prefect_angle)
							{
								if (local_player->GetVelocity().Length2D() < 0.1f)
									yaw = UTILS::GetLBYRotatedYaw(yaw + lby_delta, yaw);
								else
									yaw = UTILS::GetLBYRotatedYaw(local_player->GetLowerBodyYaw(), yaw);
							}

							lby = yaw + lby_delta;

							if (fabs(MATH::NormalizeYaw(local_player->GetLowerBodyYaw() - lby)) < 35.f)
							{
								yaw = local_player->GetLowerBodyYaw() - lby_delta;
								lby = yaw + lby_delta;
							}

							return true;
						};
					auto DoBackFreestanding = [at_target_yaw, local_player](float& yaw, float& lby) -> bool
						{
							yaw = at_target_yaw + 180.f;
							lby = yaw;

							return true;
						};

					const auto head_position = local_player->GetVecOrigin() + Vector(0, 0, 74.f);
					const auto back_head_position = RotateAndExtendPosition(head_position, at_target_yaw + 180.f, 17.f);
					auto right_head_position = RotateAndExtendPosition(head_position, right_yaw, 17.f);
					auto left_head_position = RotateAndExtendPosition(head_position, left_yaw, 17.f);

					int right_damage = CalcDamage(right_head_position), left_damage = CalcDamage(left_head_position), back_damage = CalcDamage(back_head_position);

					/// too much damage to both sides
					if (right_damage > damage_tolerance && left_damage > damage_tolerance)
					{
						/// do backwards if valid
						if (back_damage < damage_tolerance)
							return DoBackFreestanding(yaw, lby);

						return false;
					}

					/// keep searching for a better angle
					if (right_damage == left_damage)
					{
						/// if on top of them, prioritise backwards antiaim
						if (MATH::NormalizePitch(MATH::CalcAngle(local_player->GetVecOrigin(), closest_enemy->GetVecOrigin()).x) > 15.f && back_damage < damage_tolerance)
							return DoBackFreestanding(yaw, lby);

						/// do some traces a bit further out
						right_head_position = RotateAndExtendPosition(head_position, right_yaw, 50.f);
						left_head_position = RotateAndExtendPosition(head_position, left_yaw, 50.f);

						right_damage = CalcDamage(right_head_position) , left_damage = CalcDamage(left_head_position);
						if (right_damage == left_damage)
						{
							/// just return the side closest to a wall
							right_head_position = UTILS::TraceToEnd(head_position, RotateAndExtendPosition(head_position, right_yaw, 17.f));
							left_head_position = UTILS::TraceToEnd(head_position, RotateAndExtendPosition(head_position, left_yaw, 17.f));

							float distance_1, distance_2;
							SDK::trace_t trace;
							SDK::Ray_t ray;
							SDK::CTraceWorldOnly filter;
							const auto end_pos = closest_enemy->GetVecOrigin() + Vector(0, 0, 64.f);

							/// right position
							ray.Init(right_head_position, end_pos);
							INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
							distance_1 = (right_head_position - trace.end).Length();

							/// left position
							ray.Init(left_head_position, end_pos);
							INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
							distance_2 = (left_head_position - trace.end).Length();

							if (fabs(distance_1 - distance_2) > 15.f)
							{
								RotateLBYAndYaw(distance_1, distance_2, SETTINGS::ragebot_configs.antiaim_edging_lby_delta, yaw, lby);

								if ((is_moving && !UTILS::IsPressingMovementKeys(reinterpret_cast<SDK::CUserCmd*>(GLOBAL::last_cmd))) ||
									(UTILS::GetCurtime() - m_last_move_time < 0.22 && !is_moving))
									yaw = lby + 60.f;

								return true;
							}

							return DoBackFreestanding(yaw, lby);
						}
						else
						{
							RotateLBYAndYaw(right_damage, left_damage, SETTINGS::ragebot_configs.antiaim_edging_lby_delta, yaw, lby);

							if ((is_moving && !UTILS::IsPressingMovementKeys(reinterpret_cast<SDK::CUserCmd*>(GLOBAL::last_cmd))) ||
								(UTILS::GetCurtime() - m_last_move_time < 0.22 && !is_moving))
								yaw = lby + 60.f;

							return true;
						}
					}
					else /// found an angle that does less damage than the other
					{
						/// if on top of them, prioritise backwards antiaim
						if (MATH::NormalizePitch(MATH::CalcAngle(local_player->GetVecOrigin(), closest_enemy->GetVecOrigin()).x) > 15.f && back_damage < damage_tolerance)
							return DoBackFreestanding(yaw, lby);

						const bool prefer_right = (right_damage < left_damage);
						const float lby_delta = SETTINGS::ragebot_configs.antiaim_edging_lby_delta;

						yaw = prefer_right ? right_yaw : left_yaw;
						lby = yaw + lby_delta;

						if (fabs(MATH::NormalizeYaw(local_player->GetLowerBodyYaw() - lby)) < 35.f)
						{
							yaw = local_player->GetLowerBodyYaw() - lby_delta;
							lby = yaw + lby_delta;
						}

						if (CalcDamage(RotateAndExtendPosition(head_position, lby, 18.f)) > 0)
						{
							lby = yaw + (prefer_right ? -115.f : 115.f);
							if (CalcDamage(RotateAndExtendPosition(head_position, lby, 18.f)) > 0)
								lby = yaw;
						}
						else
						{
							if ((is_moving && !UTILS::IsPressingMovementKeys(reinterpret_cast<SDK::CUserCmd*>(GLOBAL::last_cmd))) ||
								(UTILS::GetCurtime() - m_last_move_time < 0.22 && !is_moving))
								yaw = lby + 60.f;
						}

						return true;
					}

					return false;
				};

			const int edging_type = SETTINGS::ragebot_configs.antiaim_edging_type;
			switch (edging_type)
			{
				case 0: /// wall detection
					return DoWallDetection(yaw, lby);
				case 1: /// freestanding
					if (DoFreestanding(yaw, lby))
						return true;

					return false;
				case 2: /// freestanding + wall detection
					if (DoWallDetection(yaw, lby))
						return true;

					return DoFreestanding(yaw, lby);
			}

			return false;
		}
	}
}
