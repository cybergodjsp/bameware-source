#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CUserCmd.h"

#include "Configurations.h"

#include "Autostrafer.h"

namespace FEATURES
{
	namespace MISC
	{
		Autostrafer autostrafer;

		void Autostrafer::Do(SDK::CUserCmd* cmd)
		{
			if (!SETTINGS::main_configs.autostrafer_enabled)
				return;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0 || (local_player->GetFlags() & FL_ONGROUND && !(cmd->buttons & IN_JUMP)))
				return;

			//auto get_move_angle = [](float speed) -> float
			//{
			//	const auto move_angle = RAD2DEG(asin(15.f / speed));
			//
			//	return (!isfinite(move_angle) || move_angle > 90.f) ? 90.f : 0.f;
			//};
			//
			//static auto switch_key = 1.f;
			//static auto old_yaw = 0.f;
			//
			//auto velocity = local_player->GetVelocity();
			//velocity.z = 0.f;
			//
			//auto speed = velocity.Length2D();
			//auto ideal_strafe = get_move_angle(speed);
			//
			Vector viewangles;
			INTERFACES::Engine->GetViewAngles(viewangles);
			//
			//const auto yaw_delta = MATH::NormalizeYaw(cmd->viewangles.x - old_yaw);
			//const auto abs_yaw_delta = fabs(yaw_delta);
			//
			//yaw_delta > 0.f ? cmd->sidemove = -450.f : cmd->sidemove = 450.f;
			//
			//if (abs_yaw_delta <= ideal_strafe || abs_yaw_delta >= 30.f)
			//{
			//	Vector vel_angles;
			//	MATH::VectorAngles(velocity, vel_angles);
			//
			//	auto velocity_delta = MATH::NormalizeYaw(cmd->viewangles.y - vel_angles.y);
			//	auto retrack = 2.f * (ideal_strafe * 2.f);
			//
			//	if (velocity_delta <= retrack || speed <= 15.f)
			//	{
			//		if (-retrack <= velocity_delta || speed <= 15.f)
			//		{
			//			cmd->viewangles.y += ideal_strafe * switch_key;
			//			cmd->sidemove = switch_key * 450.f;
			//		}
			//		else
			//		{
			//			cmd->viewangles.y = vel_angles.y - retrack;
			//			cmd->sidemove = 450.f;
			//		}
			//	}
			//	else
			//	{
			//		cmd->viewangles.y = vel_angles.y + retrack;
			//		cmd->sidemove = -450.f;
			//	}
			//}
			
			static bool side_switch = false;
			side_switch = !side_switch;
			
			cmd->forwardmove = 0.f;
			cmd->sidemove = side_switch ? 450.f : -450.f;
			
			const float velocity_yaw = MATH::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y;
			const float ideal_rotation = GetIdealRotation(local_player->GetVelocity().Length2D()) / INTERFACES::Globals->interval_per_tick;
			const float ideal_yaw_rotation = (side_switch ? ideal_rotation : -ideal_rotation) + (fabs(MATH::NormalizeYaw(velocity_yaw - viewangles.y)) < 5.f ? velocity_yaw : viewangles.y);
			
			UTILS::RotateMovement(cmd, ideal_yaw_rotation);
		}
	}
}
