#pragma once

namespace FEATURES
{
	namespace LEGITBOT
	{
		class LegitAimbot
		{
		public:
			void Do(SDK::CUserCmd* cmd);
			void Update(SDK::CUserCmd* cmd);

		private:
			float duration_spent_clicking = 0.f;
			Vector last_viewangles;
		};

		extern LegitAimbot legit_aimbot;
	}
}
