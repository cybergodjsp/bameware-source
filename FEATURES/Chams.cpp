#include "../includes.h"

#include "Chams.h"

#include "../UTILS/interfaces.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/ModelRender.h"
#include "../SDK/IEngine.h"
#include "../SDK/IMaterial.h"

#include "Configurations.h"
#include "Backtracking.h"

namespace FEATURES
{
	namespace VISUALS
	{
		CChams chams;

		void CChams::DoSceneEnd()
		{

		}

		void CChams::DrawBacktrackPlayer(SDK::CBaseEntity* entity)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam() || !SETTINGS::main_configs.bt_vis_chams_enabled)
				return;

			auto records = FEATURES::RAGEBOT::backtracking.GetValidRecords(entity);
			if (!records.size())
				return;

			FEATURES::RAGEBOT::backtracking.Restore(entity, records.back());
			FEATURES::RAGEBOT::backtracking.ApplyRestore(entity, records.back().curtime);

			INTERFACES::ModelRender->ForcedMaterialOverride(backtrack_visualization_mat);
			entity->DrawModel(0x1, 255);

			FEATURES::RAGEBOT::backtracking.RestoreToCurrentRecord(entity);
		}

		void CChams::DrawPlayer(SDK::CBaseEntity* entity)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			bool is_localplayer = local_player == entity;
			bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_localplayer;
			bool is_enemy = local_player->GetTeam() != entity->GetTeam();

			// if doesn't have chams enabled
			bool should_draw_chams = true;
			if (is_localplayer && !SETTINGS::main_configs.chams_local_enabled)
				should_draw_chams = false;
			else if (is_teammate && !SETTINGS::main_configs.chams_team_enabled)
				should_draw_chams = false;
			else if (is_enemy && !SETTINGS::main_configs.chams_enemy_enabled)
				should_draw_chams = false;

			SDK::IMaterial* occluded_material = nullptr;
			SDK::IMaterial* visible_material = nullptr;

			if (is_localplayer)
				visible_material = local_player_mat;
			else if (is_teammate)
			{
				occluded_material = teammate_occluded_mat;
				visible_material = teammate_visible_mat;
			}
			else if (is_enemy)
			{
				occluded_material = enemy_occluded_mat;
				visible_material = enemy_visible_mat;
			}

			/// dont draw chams on localplayer if transparency when scoped
			if (is_localplayer && local_player->IsScopedIn() && SETTINGS::main_configs.transparency_when_scoped < 100.f)
				return;

			DrawBacktrackPlayer(entity);

			if (!is_localplayer && should_draw_chams)
			{
				INTERFACES::ModelRender->ForcedMaterialOverride(occluded_material);
				entity->DrawModel(0x1, 255);
			}

			if (should_draw_chams)
			{
				INTERFACES::ModelRender->ForcedMaterialOverride(visible_material);
				entity->DrawModel(0x1, 255);
			}
		}

		bool CChams::ShouldDrawModel(SDK::CBaseEntity* entity)
		{
			return false;
		}

		void CChams::UpdateMaterials()
		{
			static bool initilized = false;
			if (!initilized)
			{
				initilized = true;

				local_player_mat = CreateMaterial(false, true, false);
				enemy_visible_mat = CreateMaterial(false, true, false);
				enemy_occluded_mat = CreateMaterial(true, true, false);
				teammate_visible_mat = CreateMaterial(false, true, false);
				teammate_occluded_mat = CreateMaterial(true, true, false);
				backtrack_visualization_mat = CreateMaterial(true, true, false);
			}

			static unsigned int material_update_time = 0;
			const bool force_material_update = material_update_time % 500;
			material_update_time++;

			static auto last_local_color = SETTINGS::main_configs.chams_local_color;
			static auto last_enemy_occl_color = SETTINGS::main_configs.chams_enemy_occluded_color;
			static auto last_enemy_unoccl_color = SETTINGS::main_configs.chams_enemy_unoccluded_color;
			static auto last_team_occl_color = SETTINGS::main_configs.chams_team_occluded_color;
			static auto last_team_unoccl_color = SETTINGS::main_configs.chams_team_unoccluded_color;
			static auto last_backtrack_color = SETTINGS::main_configs.bt_vis_chams_color;

			/// only color modulate if they change the color
			if (SETTINGS::main_configs.chams_local_color != last_local_color || force_material_update)
			{
				last_local_color = SETTINGS::main_configs.chams_local_color;

				local_player_mat->ColorModulate(SETTINGS::main_configs.chams_local_color);
				local_player_mat->AlphaModulate(SETTINGS::main_configs.chams_local_color.RGBA[3] / 255.f);
			}
			if (SETTINGS::main_configs.chams_enemy_unoccluded_color != last_enemy_unoccl_color || SETTINGS::main_configs.chams_enemy_occluded_color != last_enemy_occl_color || force_material_update)
			{
				last_enemy_occl_color = SETTINGS::main_configs.chams_enemy_occluded_color;
				last_enemy_unoccl_color = SETTINGS::main_configs.chams_enemy_unoccluded_color;

				enemy_visible_mat->ColorModulate(SETTINGS::main_configs.chams_enemy_unoccluded_color);
				enemy_visible_mat->AlphaModulate(SETTINGS::main_configs.chams_enemy_unoccluded_color.RGBA[3] / 255.f);
				enemy_occluded_mat->ColorModulate(SETTINGS::main_configs.chams_enemy_occluded_color);
				enemy_occluded_mat->AlphaModulate(SETTINGS::main_configs.chams_enemy_occluded_color.RGBA[3] / 255.f);
			}
			if (SETTINGS::main_configs.chams_team_unoccluded_color != last_team_unoccl_color || SETTINGS::main_configs.chams_team_occluded_color != last_team_occl_color || force_material_update)
			{
				last_team_occl_color = SETTINGS::main_configs.chams_team_occluded_color;
				last_team_unoccl_color = SETTINGS::main_configs.chams_team_unoccluded_color;

				teammate_visible_mat->ColorModulate(SETTINGS::main_configs.chams_team_unoccluded_color);
				teammate_visible_mat->AlphaModulate(SETTINGS::main_configs.chams_team_unoccluded_color.RGBA[3] / 255.f);
				teammate_occluded_mat->ColorModulate(SETTINGS::main_configs.chams_team_occluded_color);
				teammate_occluded_mat->AlphaModulate(SETTINGS::main_configs.chams_team_occluded_color.RGBA[3] / 255.f);
			}
			if (SETTINGS::main_configs.bt_vis_chams_color != last_backtrack_color || force_material_update)
			{
				last_backtrack_color = SETTINGS::main_configs.bt_vis_chams_color;

				backtrack_visualization_mat->ColorModulate(SETTINGS::main_configs.bt_vis_chams_color);
				backtrack_visualization_mat->AlphaModulate(SETTINGS::main_configs.bt_vis_chams_color.RGBA[3] / 255.f);
			}

			static bool last_local_wireframe = false, last_enemy_wireframe = false, last_team_wireframe = false;

			if (last_local_wireframe != SETTINGS::main_configs.chams_local_wireframe_enabled || force_material_update)
			{
				last_local_wireframe = SETTINGS::main_configs.chams_local_wireframe_enabled;
				local_player_mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, SETTINGS::main_configs.chams_local_wireframe_enabled);
			}
			if (last_enemy_wireframe != SETTINGS::main_configs.chams_enemy_wireframe_enabled || force_material_update)
			{
				last_enemy_wireframe = SETTINGS::main_configs.chams_enemy_wireframe_enabled;
				enemy_visible_mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, SETTINGS::main_configs.chams_enemy_wireframe_enabled);
				enemy_occluded_mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, SETTINGS::main_configs.chams_enemy_wireframe_enabled);
			}
			if (last_team_wireframe != SETTINGS::main_configs.chams_team_wireframe_enabled || force_material_update)
			{
				last_team_wireframe = SETTINGS::main_configs.chams_team_wireframe_enabled;
				teammate_visible_mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, SETTINGS::main_configs.chams_team_wireframe_enabled);
				teammate_occluded_mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, SETTINGS::main_configs.chams_team_wireframe_enabled);
			}
		}

		SDK::IMaterial* CChams::CreateMaterial(bool shouldIgnoreZ, bool isLit, bool isWireframe) //credits to ph0ne
		{
			static int created = 10;
			static const char tmp[] =
			{
				"\"%s\"\
				\n{\
				\n\t\"$basetexture\" \"vgui/white_additive\"\
				\n\t\"$envmap\" \"env_cubemap\"\
				\n\t\"$model\" \"1\"\
				\n\t\"$flat\" \"0\"\
				\n\t\"$nocull\" \"0\"\
				\n\t\"$selfillum\" \"1\"\
				\n\t\"$halflambert\" \"1\"\
				\n\t\"$nofog\" \"1\"\
				\n\t\"$ignorez\" \"%i\"\
				\n\t\"$znearer\" \"0\"\
				\n\t\"$wireframe\" \"%i\"\
				\n}\n"
			};

			char* baseType = (isLit == true ? "VertexLitGeneric" : "UnlitGeneric");
			char material[512];
			sprintf_s(material, sizeof(material), tmp, baseType, (shouldIgnoreZ) ? 1 : 0, (isWireframe) ? 1 : 0);

			char name[512];
			sprintf_s(name, sizeof(name), "#nice_pener_material%i.vmt", created);
			++created;

			SDK::KeyValues* keyValues = new SDK::KeyValues;

			SDK::IMaterial* created_material = INTERFACES::MaterialSystem->CreateMaterial(name, keyValues);
			created_material->IncrementReferenceCount();

			return created_material;
		}
	}
}
