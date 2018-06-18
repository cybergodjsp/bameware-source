#pragma once

namespace SDK
{
	class CUserCmd;
}

namespace FEATURES
{
	namespace RAGEBOT
	{
		class Fakewalk
		{
		public:
			void Do(SDK::CUserCmd* cmd);

		private:
		};

		extern Fakewalk fakewalk;
	}
}
