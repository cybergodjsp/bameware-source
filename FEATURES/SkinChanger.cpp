#include "../includes.h"

#include <cctype>

#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CGlobalVars.h"

#include "SkinChanger.h"
#include "Configurations.h"

namespace FEATURES
{
	namespace MISC
	{
		SkinChanger skin_changer;

		void SkinChanger::Do()
		{
			if (!INTERFACES::Engine->IsInGame() || !INTERFACES::Engine->IsConnected())
				return;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player || local_player->GetHealth() <= 0)
				return;

			auto weapons = local_player->GetWeapons();
			if (!weapons)
				return;

			SDK::player_info_t player_info;
			if (!INTERFACES::Engine->GetPlayerInfo(INTERFACES::Engine->GetLocalPlayer(), &player_info))
				return;

			auto GetSkinInfo = [this](int weapon_index) -> Skin_Info
				{
					Skin_Info skin_info = Skin_Info();

					/// Weapon paintkit
					if (SETTINGS::skin_configs.weapon_skin[weapon_index] != -1)
						skin_info.fallback_paint_kit = skins_list_indexes[UTILS::Clamp<int>(SETTINGS::skin_configs.weapon_skin[weapon_index], 0, NUM_SKINS - 1)];

					/// Weapon quality
					if (SETTINGS::skin_configs.weapon_quality[weapon_index] != -1)
						skin_info.fallback_quality = qualities_list_indexes[UTILS::Clamp<int>(SETTINGS::skin_configs.weapon_quality[weapon_index], 0, NUM_QUALITIES - 1)];

					/// StatTrak
					if (SETTINGS::skin_configs.weapon_stat_trak_enabled[weapon_index])
						skin_info.fallback_stat_trak = SETTINGS::skin_configs.weapon_stat_trak_kills[weapon_index];

					skin_info.fallback_seed = SETTINGS::skin_configs.weapon_seed[weapon_index];
					skin_info.fallback_wear = SETTINGS::skin_configs.weapon_wear[weapon_index];
					skin_info.custom_name = SETTINGS::skin_configs.weapon_custom_name[weapon_index];

					return skin_info;
				};

			try
			{
				for (int i = 0; weapons[i] != 0xFFFFFFFF && weapons[i]; i++)
				{
					auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(weapons[i] & 0xFFF));
					if (!weapon)
						continue;

					for (int weapon_index = 0; weapon_index < NUM_WEAPONS; weapon_index++)
					{
						if (weapons_list_indexes[weapon_index] == weapon->GetItemDefinitionIndex())
						{
							ChangeSkin(weapon, GetSkinInfo(weapon_index));
							break;
						}
					}

					weapon->SetAccountID(player_info.xuidlow);
				}
			}
			catch (std::exception e)
			{
				LOG(e.what());
				throw e;
			}
		}

		void SkinChanger::ChangeModel(SDK::CBaseWeapon* weapon, const char* model)
		{
			reinterpret_cast<SDK::CBaseEntity*>(weapon)->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex(model));

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			auto viewmodel = reinterpret_cast<SDK::CBaseViewmodel*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetViewModelIndex()));
			if (!viewmodel)
				return;

			auto viewmodel_weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(viewmodel->GetWeaponIndex()));
			if (!viewmodel_weapon)
				return;

			if (viewmodel_weapon != weapon)
				return;

			const int model_index = INTERFACES::ModelInfo->GetModelIndex(model);
			viewmodel->SetModelIndex(model_index);
			weapon->SetViewModelIndex(model_index);
			weapon->SetWorldModelIndex(model_index + 1);

			//auto world_model = INTERFACES::ClientEntityList->GetClientEntity(viewmodel_weapon->GetWeaponWorldModelIndex());
			//if (!world_model)
			//return;

			//world_model->SetModelIndex(model_index + 1);

			weapon->SetItemDefinitionIndex(507);
		}

		void SkinChanger::ChangeSkin(SDK::CBaseWeapon* weapon, Skin_Info skin_info)
		{
			if (skin_info.fallback_paint_kit != -1)
				weapon->SetFallbackPaintKit(skin_info.fallback_paint_kit);

			if (skin_info.fallback_wear != -1)
				weapon->SetFallbackWear(skin_info.fallback_wear);

			if (skin_info.fallback_quality != -1)
				weapon->SetEntityQuality(skin_info.fallback_quality);

			if (skin_info.fallback_stat_trak != -1)
				weapon->SetFallbackStatTrak(skin_info.fallback_stat_trak);

			if (skin_info.fallback_seed != -1)
				weapon->SetFallbackSeed(skin_info.fallback_seed);

			if (skin_info.custom_name)
				sprintf(weapon->GetCustomName(), skin_info.custom_name);

			weapon->SetItemIDHigh(-1);
		}
	}
}
