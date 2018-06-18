#include "../includes.h"

#include "../UTILS/variables.h"
#include "../UTILS/interfaces.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/IEngine.h"
#include "../SDK/NetChannel.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseAnimState.h"

#include "../FEATURES/AntiAim.h"
#include "Fakewalk.h"

#include "Configurations.h"

namespace FEATURES
{
	namespace RAGEBOT
	{
		Fakewalk fakewalk;

		void Fakewalk::Do(SDK::CUserCmd* cmd)
		{
			GLOBAL::is_fakewalking = false;

			if (((SETTINGS::main_configs.fakewalk_keybind != -1 && GetAsyncKeyState(SETTINGS::main_configs.fakewalk_keybind)) ||
				(SETTINGS::main_configs.fakewalk_when_peek && GLOBAL::can_shoot_someone)))
			{
				auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
				if (!local_player || local_player->GetHealth() <= 0)
					return;

				auto net_channel = INTERFACES::Engine->GetNetChannel();
				if (!net_channel)
					return;

				auto animstate = local_player->GetAnimState();
				if (!animstate)
					return;

				if (!net_channel->m_nChokedPackets)
				{
					if (antiaim.TimeUntilNextLBYUpdate() > 0.15)
						GLOBAL::should_choke_packets = true;
					else
						GLOBAL::should_choke_packets = false;
				}

				GLOBAL::is_fakewalking = true;

				cmd->sidemove *= SETTINGS::main_configs.fakewalk_speed;
				cmd->forwardmove *= SETTINGS::main_configs.fakewalk_speed;

				if (GLOBAL::should_choke_packets)
				{
					const int choked_ticks = net_channel->m_nChokedPackets;

					if (!choked_ticks || choked_ticks > (animstate->speed_2d < 115.f ? 9 : 8))
					{
						cmd->sidemove = 0;
						cmd->forwardmove = 450;

						if (!choked_ticks || animstate->speed_2d < 20.f)
							cmd->forwardmove = 0;

						UTILS::RotateMovement(cmd, MATH::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y + 180.f);
					}
				}
				else
				{
					cmd->sidemove = 0;
					cmd->forwardmove = 0;
				}
			}
		}
	}
}
