#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"

#include "../FEATURES/Backtracking.h"

#include "Legitbot.h"

namespace FEATURES
{
	namespace LEGITBOT
	{
		LegitAimbot legit_aimbot;

		void LegitAimbot::Update(SDK::CUserCmd* cmd)
		{
			if (cmd->buttons & IN_ATTACK)
				duration_spent_clicking += INTERFACES::Globals->interval_per_tick;
			else
				duration_spent_clicking = 0.f;
		}

		void LegitAimbot::Do(SDK::CUserCmd* cmd)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			auto weapon = UTILS::GetWeapon(local_player);
			if (!weapon)
				return;

			auto weapon_info = weapon->GetWeaponInfo();
			if (!weapon_info)
				return;

			static constexpr int hitboxes_to_check[] = {0, 1, 2, 3, 4, 5, 6, 8, 9, 18, 16};
			static constexpr bool aimbot_enabled = false;

			auto DoAimbot = [local_player, this](Vector& viewangles, float aimbot_fov) -> void
				{
					auto CanHit = [](Vector start, Vector end, SDK::CBaseEntity* ignore) -> bool
						{
							SDK::trace_t trace;
							SDK::CTraceFilter filter;
							filter.pSkip1 = ignore;
							SDK::Ray_t ray;
							ray.Init(start, end);

							INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

							return trace.hitGroup > 0 && trace.hitGroup <= 7;
						};
					auto GetFov = [](Vector viewangles, Vector start, Vector end) -> float
						{
							const auto target_angles = MATH::NormalizeAngle(MATH::CalcAngle(start, end));
							return (viewangles - target_angles).Length2D();
						};
					auto GetHitbox = [](SDK::CBaseEntity* entity, int hitbox_index) -> SDK::mstudiobbox_t*
				
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
						};

					if (duration_spent_clicking <= 0.f || duration_spent_clicking > 0.5f)
						return;

					const auto eye_position = local_player->GetVecOrigin() + local_player->GetViewOffset();
					const auto true_viewangles = viewangles + (local_player->GetPunchAngles() * 2.f);

					bool has_target = false;
					float lowest_fov = FLT_MAX;
					Vector best_position;
					for (int i = 0; i < 64; i++)
					{
						auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
						if (!entity || entity->GetHealth() <= 0 || entity->GetIsDormant() || entity->GetImmunity() || entity->GetTeam() == local_player->GetTeam())
							continue;

						auto GetClosestHitboxPosition = [local_player, entity, eye_position, true_viewangles, CanHit, GetFov, GetHitbox]() -> Vector
							{
								float lowest_fov = FLT_MAX;
								Vector closest_position;
								for (const int i : hitboxes_to_check)
								{
									const auto hitbox = GetHitbox(entity, i);
									const auto bone_position = Vector(entity->GetBoneMatrix(hitbox->bone)[0][3], entity->GetBoneMatrix(hitbox->bone)[1][3], entity->GetBoneMatrix(hitbox->bone)[2][3]);
									const auto hitbox_position = bone_position + ((hitbox->bbmax + hitbox->bbmin) * 0.5f);

									if (const float fov = GetFov(true_viewangles, eye_position, hitbox_position);
									fov < lowest_fov && CanHit(eye_position, hitbox_position, local_player)
									)
									{
										lowest_fov = fov;
										closest_position = hitbox_position;
									}
								}

								return closest_position;
							};

						const auto closest_hitbox = GetClosestHitboxPosition();
						const float current_fov = GetFov(true_viewangles, eye_position, closest_hitbox);
						if (current_fov < lowest_fov)
						{
							lowest_fov = current_fov;
							best_position = closest_hitbox;
							has_target = true;
						}
					}

					if (!has_target || lowest_fov > aimbot_fov)
						return;

					viewangles = MATH::NormalizeAngle(MATH::CalcAngle(eye_position, best_position)) - (local_player->GetPunchAngles() * 2.f);
				};

			Vector viewangles;
			INTERFACES::Engine->GetViewAngles(viewangles);

			if (aimbot_enabled)
				DoAimbot(viewangles, ((viewangles - last_viewangles).Length() * 2.f) + 1.f);

			viewangles = MATH::NormalizeAngle(viewangles);
			INTERFACES::Engine->SetViewAngles(viewangles);

			cmd->viewangles = viewangles;
			cmd->tick_count += FEATURES::RAGEBOT::backtracking.GetLerpTicks();

			INTERFACES::Engine->GetViewAngles(last_viewangles);
		}
	}
}
