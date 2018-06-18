#include "../includes.h"

#include "SkeletonESP.h"
#include "Visuals.h"

#include "../UTILS/render.h"
#include "../UTILS/interfaces.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/IEngine.h"
#include "../FEATURES/Aimbot.h"

#include "Configurations.h"

namespace FEATURES
{
	namespace VISUALS
	{
		SkeletonESP skeleton_esp;

		void SkeletonESP::Do()
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			for (int i = 0; i < 64; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				if (!entity || entity->GetHealth() <= 0 || entity == local_player)
					continue;

				/// skeleton esp not enabled, continue
				if (entity->GetTeam() == local_player->GetTeam() && !SETTINGS::main_configs.esp_team_skeleton_enabled)
					continue;
				if (entity->GetTeam() != local_player->GetTeam() && !SETTINGS::main_configs.esp_enemy_skeleton_enabled)
					continue;

				/// asign the color
				CColor color = SETTINGS::main_configs.esp_enemy_skeleton_color;
				if (entity->GetTeam() == local_player->GetTeam())
					color = SETTINGS::main_configs.esp_team_skeleton_color;

				/// random shit for dormant, pulsing alpha
				float pulse = fabs(sin(GetTickCount() * 0.003f));
				float start_fade_alpha = min(color.RGBA[3], 150.f);
				float alpha;
				if (entity->GetIsDormant())
					alpha = (start_fade_alpha - ((player_time_dormant[i] / FADE_OUT_TIME) * (255 - start_fade_alpha))) * pulse;
				else
					alpha = start_fade_alpha + ((player_time_not_dormant[i] / FADE_IN_TIME) * (255 - start_fade_alpha));

				color.RGBA[3] = UTILS::Clamp<int>(alpha, 0, color.RGBA[3]);

				/// draw the skeleton
				DrawSkeleton(entity, color);
			}
		}

		void SkeletonESP::DrawSkeleton(SDK::CBaseEntity* entity, CColor color, const matrix3x4_t* bone_matrix)
		{
			auto studio_model = INTERFACES::ModelInfo->GetStudioModel(entity->GetModel());
			if (!studio_model)
				return;

			for (int i = 0; i < studio_model->numbones; i++)
			{
				auto bone = studio_model->GetBone(i);
				if (!bone || bone->parent < 0 || !(bone->flags & BONE_USED_BY_HITBOX))
					continue;

				/// 2 bone matrices, 1 for the child and 1 for the parent bone
				matrix3x4_t bone_matrix_1, bone_matrix_2;
				if (bone_matrix)
				{
					bone_matrix_1 = bone_matrix[i];
					bone_matrix_2 = bone_matrix[bone->parent];
				}
				else
				{
					bone_matrix_1 = entity->GetBoneMatrix(i);
					bone_matrix_2 = entity->GetBoneMatrix(bone->parent);
				}

				Vector bone_position_1 = Vector(bone_matrix_1[0][3], bone_matrix_1[1][3], bone_matrix_1[2][3]),
					bone_position_2 = Vector(bone_matrix_2[0][3], bone_matrix_2[1][3], bone_matrix_2[2][3]);

				if (Vector screen1
				,
				screen2;
				RENDER::WorldToScreen(bone_position_1, screen1) && RENDER::WorldToScreen(bone_position_2, screen2)
				)
				RENDER::DrawLine(screen1.x, screen1.y, screen2.x, screen2.y, color);
			}
		}
	}
}
