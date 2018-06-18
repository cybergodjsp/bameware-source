#pragma once

namespace SDK
{
	class CUserCmd;
	class CBaseEntity;
}

namespace FEATURES
{
	namespace RAGEBOT
	{
		class Antiaim
		{
		public:
			void Do(SDK::CUserCmd* cmd);
			void UpdateLBY();

			float TimeUntilNextLBYUpdate()
			{
				return m_next_lby_update_time - UTILS::GetCurtime();
			}

		private:
			float m_next_lby_update_time = 0.f, m_last_move_time = 0.f, m_last_attempted_lby = 0.f;
			bool m_will_lby_update = false;

		private:
			float DoYaw();
			float DoPitch();

			bool DoEdging(float& yaw, float& lby);

			float BreakLBY(float real_yaw, float desired_lby, bool double_flick = false);
		};

		extern Antiaim antiaim;
	}
}
