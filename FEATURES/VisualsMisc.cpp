#include "../includes.h"

#include "../UTILS/interfaces.h"

#include "../UTILS/render.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/ConVar.h"
#include "../SDK/CGlobalVars.h"

#include "VisualsMisc.h"

#include "../FEATURES/Autowall.h"
#include "../FEATURES/AntiAim.h"
#include "../FEATURES/Configurations.h"

namespace FEATURES
{
	namespace VISUALS
	{
		VisualsMisc visuals_misc;

		void VisualsMisc::Do()
		{
			if (!INTERFACES::Engine->IsConnected() || !INTERFACES::Engine->IsInGame())
				return;

			DrawGrenadePrediction();
			DrawSnaplines();
			DrawSpreadCircle();
			DrawCrosshair();
		}

		void VisualsMisc::DisableFlashDuration()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			if (SETTINGS::main_configs.no_flash_enabled)
				local_player->SetFlashDuration(0.f);
		}

		void VisualsMisc::DrawGrenadePrediction()
		{
			if (!SETTINGS::main_configs.grenade_tracers_enabled)
				return;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
			if (!weapon)
				return;

			auto weapon_info = weapon->GetWeaponInfo();
			if (!weapon_info)
				return;

			if (weapon_info->m_WeaponType != WEAPON_TYPE_GRENADE)
				return;

			static auto molotov_throw_detonate_time = INTERFACES::cvar->FindVar("molotov_throw_detonate_time");
			static auto weapon_molotov_maxdetonateslope = INTERFACES::cvar->FindVar("weapon_molotov_maxdetonateslope");
			static auto sv_gravity = INTERFACES::cvar->FindVar("sv_gravity");

			auto GetGrenadeDetonateTime = [](int item)
				{
					switch (item)
					{
						case SDK::weapon_flashbang:
						case SDK::weapon_hegrenade:
							return 1.5f;
						case SDK::weapon_incgrenade:
						case SDK::weapon_molotov:
							return molotov_throw_detonate_time->GetFloat();
						case SDK::weapon_tagrenade:
							return 5.f;
					}

					return 3.f;
				};
			auto DrawLine = [](Vector start, Vector end)
				{
					Vector screen1, screen2;

					if (!RENDER::WorldToScreen(start, screen1)
						|| !RENDER::WorldToScreen(end, screen2))
						return;

					RENDER::DrawLine(screen1.x, screen1.y, screen2.x, screen2.y, SETTINGS::main_configs.grenade_tracers_color);
				};
			auto PhysicsClipVelocity = [](const Vector& in, const Vector& normal, Vector& out, float overbounce)
				{
					int blocked = 0;
					float angle = normal[2];

					if (angle > 0)
						blocked |= 1; // floor

					if (!angle)
						blocked |= 2; // step

					float backoff = in.Dot(normal) * overbounce;

					for (int i = 0; i < 3; i++)
					{
						out[i] = in[i] - (normal[i] * backoff);

						if (out[i] > -0.1f && out[i] < 0.1f)
							out[i] = 0;
					}

					return blocked;
				};

			Vector throw_angle, direction;
			INTERFACES::Engine->GetViewAngles(throw_angle);

			throw_angle.x = MATH::NormalizeYaw(throw_angle.x);
			throw_angle.x -= (90.f - fabs(throw_angle.x)) * 0.11111111f;

			MATH::AngleVectors(throw_angle, &direction);

			float throw_strength = *reinterpret_cast<float*>(uintptr_t(weapon) + OFFSETS::m_flThrowStrength);
			float throw_velocity = fmin(fmax(750.f * 0.9f, 15.f), 750.f);

			float throwHeight = (throw_strength * 12.f) - 12.f;
			float v68 = throw_velocity * ((0.7f * throw_strength) + 0.3f);

			Vector start_position = local_player->GetVecOrigin() + local_player->GetViewOffset() + Vector(0, 0, throwHeight), end_position = start_position + (direction * 22.f);

			SDK::trace_t trace;
			SDK::CTraceFilter filter;
			filter.pSkip1 = local_player;
			SDK::Ray_t ray;
			ray.Init(start_position, end_position);

			INTERFACES::Trace->TraceRay(ray, MASK_SOLID | CONTENTS_CURRENT_90, &filter, &trace);

			end_position = trace.end - (direction * 6.f);
			auto throw_position = (local_player->GetVelocity() * 1.25f) + (direction * v68);

			DrawLine(start_position, end_position);
			float gravity = (sv_gravity->GetFloat() * 0.4f);

			const int start_tick = TIME_TO_TICKS(GetGrenadeDetonateTime(weapon->GetItemDefinitionIndex()));
			for (int ticks = start_tick; 0 <= ticks; --ticks)
			{
				auto throw_direction = Vector(throw_position.x, throw_position.y, (throw_position.z + (throw_position.z - (gravity * INTERFACES::Globals->interval_per_tick))) * 0.5f);

				auto temp = throw_direction * INTERFACES::Globals->interval_per_tick;

				throw_position.z -= gravity * INTERFACES::Globals->interval_per_tick;

				auto src = end_position, end = end_position + temp;

				ray.Init(src, end);
				INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

				if (trace.allsolid)
					throw_position = Vector(0, 0, 0);

				end_position = trace.end;

				/// hit a player
				if (trace.hitGroup > 0 && trace.hitGroup <= 7)
				{
					if (Vector screen;
					RENDER::WorldToScreen(end_position, screen)
					)
					RENDER::DrawFilledCircle(screen.x, screen.y, 4, 50, RED);

					break;
				}

				/// dont draw the first line
				if (ticks != start_tick)
					DrawLine(src, end_position);

				/// draw the dots at the ends of the lines
				if (trace.flFraction != 1.f)
				{
					if (Vector screen;
					RENDER::WorldToScreen(end_position, screen)
					)
					RENDER::DrawFilledCircle(screen.x, screen.y, 2, 50, WHITE);
				}
				else if (ticks == 0)
				{
					if (Vector screen;
					RENDER::WorldToScreen(end_position, screen)
					)
					RENDER::DrawFilledCircle(screen.x, screen.y, 3, 50, WHITE);
				}

				if (trace.flFraction != 1.f)
				{
					float surfaceElasticity = 1.f;

					Vector throwPos2;

					PhysicsClipVelocity(throw_position, trace.plane.normal, throwPos2, 2.f);

					SDK::player_info_t player_info;
					if (trace.m_pEnt && INTERFACES::Engine->GetPlayerInfo(trace.m_pEnt->GetIndex(), &player_info))
						surfaceElasticity = 0.3f;

					throwPos2 *= UTILS::Clamp(surfaceElasticity * 0.45f, 0.f, 0.9f);

					end = end_position + (throwPos2 * ((1.f - trace.flFraction) * INTERFACES::Globals->interval_per_tick));

					if (weapon->GetItemDefinitionIndex() == SDK::weapon_molotov
						|| weapon->GetItemDefinitionIndex() == SDK::weapon_incgrenade)
					{
						if (trace.plane.normal.z >= cos(DEG2RAD(weapon_molotov_maxdetonateslope->GetFloat())))
							return;
					}

					// NOTE: the tactical awareness grenade sticks to surfaces
					if (weapon->GetItemDefinitionIndex() == SDK::weapon_tagrenade)
						return;

					ray.Init(end_position, end);
					INTERFACES::Trace->TraceRay(ray, MASK_SOLID | CONTENTS_CURRENT_90, &filter, &trace);

					DrawLine(end_position, end);

					end_position = trace.end;
					throw_position = throwPos2;
				}
			}
		}

		void VisualsMisc::DrawSnaplines()
		{
			if (!SETTINGS::main_configs.snaplines_enabled)
				return;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			int width, height;
			INTERFACES::Engine->GetScreenSize(width, height);

			for (int i = 0; i < 64; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam() || entity->GetIsDormant())
					continue;

				Vector screen;
				if (RENDER::WorldToScreen(entity->GetVecOrigin(), screen))
					RENDER::DrawLine(width * 0.5f, height, screen.x, screen.y, SETTINGS::main_configs.snaplines_color);
			}
		}

		void VisualsMisc::DrawSpreadCircle()
		{
			if (!SETTINGS::main_configs.spread_circle_enabled)
				return;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
			if (!weapon)
				return;

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

			vEnd = local_player->GetVecOrigin() + local_player->GetViewOffset() + (vEnd * 8192.f);

			int width, height;
			INTERFACES::Engine->GetScreenSize(width, height);

			Vector screen;
			if (!RENDER::WorldToScreen(vEnd, screen))
				return;

			int radius = (Vector(width * 0.5f, height * 0.5f, 0) - screen).Length2D();
			if (radius <= 0)
				return;

			RENDER::DrawFilledCircle(width * 0.5f, height * 0.5f, radius, 8, SETTINGS::main_configs.spread_circle_color);
		}

		void VisualsMisc::DrawCrosshair()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			if (local_player->IsScopedIn() && SETTINGS::main_configs.no_scope_overlay_enabled)
			{
				int width, height;
				INTERFACES::Engine->GetScreenSize(width, height);

				RENDER::DrawLine(0, height * 0.5f, width * 0.5f - 5, height * 0.5f, SETTINGS::main_configs.no_scope_overlay_color);
				RENDER::DrawLine(width * 0.5f + 5, height * 0.5f, width, height * 0.5f, SETTINGS::main_configs.no_scope_overlay_color);

				RENDER::DrawLine(width * 0.5f, 0, width * 0.5f, height * 0.5f - 5, SETTINGS::main_configs.no_scope_overlay_color);
				RENDER::DrawLine(width * 0.5f, height * 0.5f + 5, width * 0.5f, height, SETTINGS::main_configs.no_scope_overlay_color);
			}

			DrawAutowallCrosshair();
		}

		void VisualsMisc::DrawAutowallCrosshair()
		{
			if (!SETTINGS::main_configs.autowall_crosshair_enabled)
				return;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
			if (!weapon)
				return;

			auto weapon_info = weapon->GetWeaponInfo();
			if (!weapon_info)
				return;

			if (weapon_info->m_WeaponType == WEAPON_TYPE_GRENADE || weapon_info->m_WeaponType == WEAPON_TYPE_KNIFE)
				return;

			Vector viewangles;
			INTERFACES::Engine->GetViewAngles(viewangles);

			Vector direction;
			MATH::AngleVectors(viewangles, &direction);

			Vector start = local_player->GetVecOrigin() + local_player->GetViewOffset();
			Vector end = start + (direction * 8192.f);

			auto autowall_info = FEATURES::RAGEBOT::autowall.CalculateDamage(start, end, local_player);
			CColor crosshair_color;
			switch (autowall_info.penetration_count)
			{
				case 0:
					crosshair_color = BLACK;
					break;
				case 1:
					crosshair_color = BLUE;
					break;
				case 2:
					crosshair_color = YELLOW;
					break;
				case 3:
					crosshair_color = GREEN;
					break;
				default:
					crosshair_color = RED;
			}

			int width, height;
			INTERFACES::Engine->GetScreenSize(width, height);

			RENDER::DrawFilledRect(width * 0.5 - 1, height * 0.5 - 1, width * 0.5 + 1, height * 0.5 + 1, crosshair_color);

			if (autowall_info.damage > 0)
				RENDER::DrawF(width * 0.5f + 20, height * 0.5f + 20, FONTS::visuals_esp_font, true, true, WHITE, std::to_string(autowall_info.damage));
		}
	}
}
