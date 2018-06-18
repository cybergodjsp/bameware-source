#pragma once

namespace SDK
{
	// forward declare shit
	class CBaseEntity;
}

namespace FEATURES
{
	namespace VISUALS
	{
		static constexpr float FADE_IN_TIME = 0.25;
		static constexpr float FADE_OUT_TIME = 5;
		extern float player_time_dormant[64];
		extern float player_time_not_dormant[64];

		void Do(); // wrapper for everything
	}
}
