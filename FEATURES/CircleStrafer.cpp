#include "../includes.h"

#include "../Utils/interfaces.h"
#include "../UTILS/render.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CTrace.h"
#include "../SDK/Collideable.h"
#include "../SDK/ConVar.h"

#include "Autostrafer.h"
#include "Configurations.h"

#include "CircleStrafer.h"

namespace FEATURES
{
	namespace MISC
	{
		CircleStrafer circle_strafer;

		void CircleStrafer::Do(SDK::CUserCmd* cmd)
		{
			if (SETTINGS::main_configs.circle_strafer_keybind == -1 || !GetAsyncKeyState(SETTINGS::main_configs.circle_strafer_keybind))
				return;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0 || local_player->GetFlags() & FL_ONGROUND)
				return;

			static bool clock_wise = false;

			const float velocity_yaw = MATH::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y;
			const float ideal_rotation_amount = Autostrafer::GetIdealRotation(local_player->GetVelocity().Length2D()) * (clock_wise ? -1.f : 1.f);

			float rotation_amount = VerifyRotation(ideal_rotation_amount);

			cmd->forwardmove = 0.f;
			cmd->sidemove = (rotation_amount > 0) ? -450.f : 450.f;
			UTILS::RotateMovement(cmd, velocity_yaw + rotation_amount);
		}

		float CircleStrafer::VerifyRotation(float ideal_rotation)
		{
			static constexpr float ray_length = 1000.f;
			static constexpr float minimum_distance = 50.f;
			static constexpr float rotation_step = 5.f;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return ideal_rotation;

			auto collideable = local_player->GetCollideable();
			if (!collideable)
				return ideal_rotation;

			auto TraceRayBoundingBox = [](Vector start, Vector end, Vector min, Vector max, float& fraction) -> void
				{
					Vector starts[8];
					Vector ends[8];

					starts[0] = start + Vector(min.x, min.y, min.z); /// min
					starts[1] = start + Vector(max.x, min.y, min.z);
					starts[2] = start + Vector(max.x, max.y, min.z);
					starts[3] = start + Vector(max.x, max.y, max.z); /// max
					starts[4] = start + Vector(min.x, max.y, max.z);
					starts[5] = start + Vector(min.x, min.y, max.z);
					starts[6] = start + Vector(max.x, min.y, max.z);
					starts[7] = start + Vector(min.x, max.y, min.z);

					ends[0] = end + Vector(min.x, min.y, min.z); /// min
					ends[1] = end + Vector(max.x, min.y, min.z);
					ends[2] = end + Vector(max.x, max.y, min.z);
					ends[3] = end + Vector(max.x, max.y, max.z); /// max
					ends[4] = end + Vector(min.x, max.y, max.z);
					ends[5] = end + Vector(min.x, min.y, max.z);
					ends[6] = end + Vector(max.x, min.y, max.z);
					ends[7] = end + Vector(min.x, max.y, min.z);

					float fractions[8];
					for (int i = 0; i < 8; i++)
					{
						SDK::CTraceWorldOnly filter;
						SDK::trace_t trace;
						SDK::Ray_t ray;
						ray.Init(starts[i], ends[i]);

						INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
						fractions[i] = trace.flFraction;
					}

					fraction = 1.f;
					for (const float& frac : fractions)
					{
						if (frac < fraction)
							fraction = frac;
					}
				};

			Vector bbmin, bbmax;
			bbmin = collideable->Mins();
			bbmax = collideable->Maxs();

			const Vector velocity_angle = MATH::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity());
			float verified_roation = ideal_rotation;

			for (float i = ideal_rotation; ((ideal_rotation > 0) ? (i < 180.f) : (i > -180.f)); ((ideal_rotation > 0) ? (i += rotation_step) : (i -= rotation_step)))
			{
				Vector direction;
				float fraction_1a, fraction_1b;
				float fraction_2a, fraction_2b;

				MATH::AngleVectors(Vector(0.f, velocity_angle.y + i, 0), &direction);
				TraceRayBoundingBox(local_player->GetVecOrigin(), local_player->GetVecOrigin() + (direction * ray_length), bbmin, bbmax, fraction_1a);
				MATH::AngleVectors(Vector(0.f, velocity_angle.y + i + rotation_step, 0), &direction);
				TraceRayBoundingBox(local_player->GetVecOrigin(), local_player->GetVecOrigin() + (direction * ray_length), bbmin, bbmax, fraction_1b);

				MATH::AngleVectors(Vector(0.f, velocity_angle.y - i, 0), &direction);
				TraceRayBoundingBox(local_player->GetVecOrigin(), local_player->GetVecOrigin() + (direction * ray_length), bbmin, bbmax, fraction_2a);
				MATH::AngleVectors(Vector(0.f, velocity_angle.y - (i + rotation_step), 0), &direction);
				TraceRayBoundingBox(local_player->GetVecOrigin(), local_player->GetVecOrigin() + (direction * ray_length), bbmin, bbmax, fraction_2b);

				if ((fraction_1a * ray_length > minimum_distance) && (fraction_1b * ray_length > minimum_distance))
				{
					verified_roation = i;
					break;
				}
				else if ((fraction_2a * ray_length > minimum_distance) && (fraction_2b * ray_length > minimum_distance))
				{
					verified_roation = -i;
					break;
				}
			}

			return verified_roation;
		}

		void CircleStrafer::VisualizeRotation(float rotation, float length)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			const float velocity_yaw = MATH::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y;

			Vector direction_1, direction_2;
			MATH::AngleVectors(Vector(0, velocity_yaw + rotation, 0), &direction_1);
			MATH::AngleVectors(Vector(0, velocity_yaw, 0), &direction_2);

			Vector screen;
			if (RENDER::WorldToScreen(local_player->GetVecOrigin() + (direction_1 * length), screen))
				RENDER::render_queue.AddDrawCircle(RENDER::RenderQueue::DrawCircleQueue(screen.x, screen.y, 2, 50, RED));
			if (RENDER::WorldToScreen(local_player->GetVecOrigin() + (direction_2 * length), screen))
				RENDER::render_queue.AddDrawCircle(RENDER::RenderQueue::DrawCircleQueue(screen.x, screen.y, 2, 50, WHITE));
		}
	}
}
