#include "../includes.h"

#include "Visuals.h"

#include "../UTILS/interfaces.h"
#include "../UTILS/render.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/IVDebugOverlay.h"
#include "../SDK/Collideable.h"
#include "../SDK/player_info_t.h"
#include "../SDK/CGlobalVars.h"
#include "../FEATURES/Backtracking.h"
#include "../FEATURES/Resolver.h"

#include "BoxESP.h"
#include "SkeletonESP.h"
#include "VisualizeBacktrack.h"
#include "VisualsMisc.h"

namespace FEATURES
{
	namespace VISUALS
	{
		float player_time_dormant[64];
		float player_time_not_dormant[64];

		void Do()
		{
			if (!INTERFACES::Engine->IsConnected() || !INTERFACES::Engine->IsInGame())
				return;

			static unsigned int last_time;

			const auto delta_time = (GetTickCount() - last_time) * 0.001f; // time since last call
			last_time = GetTickCount();

			GLOBAL::synced_rainbow_color = CColor::HSBtoRGB(fmod(last_time * 0.0002f, 1.f), 1.f, 0.5f, 255);

			// do dormant shit etc etc for fade esp and stufferz
			for (int i = 0; i < 64; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetHealth() <= 0)
					continue;

				if (entity->GetIsDormant())
				{
					player_time_not_dormant[i] = 0;
					player_time_dormant[i] += delta_time;
				}
				else
				{
					player_time_dormant[i] = 0;
					player_time_not_dormant[i] += delta_time;
				}
			}

			box_esp.Do();
			skeleton_esp.Do();
			visualize_backtrack.Do();
			visuals_misc.Do();
		}
	}
}
