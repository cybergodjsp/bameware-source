#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../UTILS/render.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/IEngine.h"

#include "../FEATURES/Backtracking.h"

#include "Visuals.h"
#include "SkeletonESP.h"
#include "VisualizeBacktrack.h"
#include "Configurations.h"

namespace FEATURES
{
	namespace VISUALS
	{
		VisualizeBacktrack visualize_backtrack;

		void VisualizeBacktrack::Do()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			for (int i = 0; i < 64; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam() || entity->GetIsDormant())
					continue;

				auto backtrack_records = RAGEBOT::backtracking.GetPriorityRecords(entity);
				for (const auto& record : backtrack_records)
				{
					/// skeleton
					if (SETTINGS::main_configs.bt_vis_skeleton_enabled)
						skeleton_esp.DrawSkeleton(entity, SETTINGS::main_configs.bt_vis_skeleton_color, record.bone_matrix);

					/// dot
					if (SETTINGS::main_configs.bt_vis_dot_enabled)
					{
						if (Vector screen;
						RENDER::WorldToScreen(Vector(record.bone_matrix[8][0][3], record.bone_matrix[8][1][3], record.bone_matrix[8][2][3]), screen)
						)
						RENDER::DrawFilledCircle(screen.x, screen.y, 2, 50, SETTINGS::main_configs.bt_vis_dot_color);
					}
				}
			}
		}
	}
}
