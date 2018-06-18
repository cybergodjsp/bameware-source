#pragma once

namespace SDK
{
	class CUserCmd;
}

namespace FEATURES
{
	namespace MISC
	{
		class Autostrafer
		{
		public:
			void Do(SDK::CUserCmd* cmd);

			static inline float GetIdealRotation(float speed)
			{
				return UTILS::Clamp<float>(RAD2DEG(std::atan2(15.f, speed)), 0.f, 45.f) * INTERFACES::Globals->interval_per_tick;
			} /// per tick

		private:
		};

		extern Autostrafer autostrafer;
	}
}
