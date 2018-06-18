#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CBaseEntity.h"

#include "../FEATURES/Backtracking.h"
#include "../FEATURES/Resolver.h"
#include "../FEATURES/InGameLogger.h"
#include "../FEATURES/Misc.h"

#include "EventListeners.h"

#include "../FEATURES/Configurations.h"

namespace FEATURES
{
	namespace MISC
	{
		CREATE_EVENT_LISTENER(ItemPurchaseListener, "item_purchase");

		CREATE_EVENT_LISTENER(PlayerHurtListener, "player_hurt");

		CREATE_EVENT_LISTENER(BulletImpactListener, "bullet_impact");

		CREATE_EVENT_LISTENER(WeaponFireListener, "weapon_fire");

		ItemPurchaseListener item_purchase_listener;
		PlayerHurtListener player_hurt_listener;
		BulletImpactListener bullet_impact_listener;
		WeaponFireListener weapon_fire_listener;

		void InitializeEventListeners()
		{
			INTERFACES::GameEventManager->AddListener(&item_purchase_listener, enc_char("item_purchase"), false);
			INTERFACES::GameEventManager->AddListener(&player_hurt_listener, enc_char("player_hurt"), false);
			INTERFACES::GameEventManager->AddListener(&bullet_impact_listener, enc_char("bullet_impact"), false);
			INTERFACES::GameEventManager->AddListener(&weapon_fire_listener, enc_char("weapon_fire"), false);
		}

		void RemoveEventListeners()
		{
			INTERFACES::GameEventManager->RemoveListener(&item_purchase_listener);
			INTERFACES::GameEventManager->RemoveListener(&player_hurt_listener);
			INTERFACES::GameEventManager->RemoveListener(&bullet_impact_listener);
			INTERFACES::GameEventManager->RemoveListener(&weapon_fire_listener);
		}

		void ItemPurchaseListener::FireGameEvent(SDK::IGameEvent* game_event)
		{
			if (!game_event)
				return;

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			auto entity = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid")));
			if (!entity)
				return;

			if (entity->GetTeam() == local_player->GetTeam())
				return;

			SDK::player_info_t player_info;
			if (!INTERFACES::Engine->GetPlayerInfo(entity->GetIndex(), &player_info))
				return;

			MISC::InGameLogger::Log log;

			log.color_line.PushBack(player_info.name, HOTPINK);
			log.color_line.PushBack(" bought a ", CColor(200, 200, 200));
			log.color_line.PushBack(game_event->GetString("weapon"), HOTPINK);

			MISC::in_game_logger.AddLog(log);
		};

		void PlayerHurtListener::FireGameEvent(SDK::IGameEvent* game_event)
		{
			if (!game_event)
				return;

			FEATURES::RAGEBOT::resolver.EventCallback(game_event);

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			auto attacker = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("attacker")));
			if (!attacker)
				return;

			if (attacker != local_player)
				return;

			if (SETTINGS::main_configs.hitmarkers_enabled)
				FEATURES::MISC::hitmarkers.AddHitmarker(game_event->GetInt("dmg_health"));

			SDK::player_info_t player_info;
			INTERFACES::Engine->GetPlayerInfo(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid")), &player_info);

			MISC::InGameLogger::Log log;

			log.color_line.PushBack("Hit ", CColor(200, 200, 200));
			log.color_line.PushBack(player_info.name, HOTPINK);

			int hitgroup = game_event->GetInt("hitgroup");
			if (hitgroup > 0 && hitgroup <= 7)
			{
				log.color_line.PushBack(" in the ", CColor(200, 200, 200));
				log.color_line.PushBack(UTILS::GetHitgroupName(hitgroup), HOTPINK);
			}

			log.color_line.PushBack(" for ", CColor(200, 200, 200));
			log.color_line.PushBack(std::to_string(game_event->GetInt("dmg_health")), HOTPINK);
			log.color_line.PushBack(" damage.", CColor(200, 200, 200));

			MISC::in_game_logger.AddLog(log);
		};

		void BulletImpactListener::FireGameEvent(SDK::IGameEvent* game_event)
		{
			if (!game_event)
				return;

			FEATURES::RAGEBOT::resolver.EventCallback(game_event);

			auto entity = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid")));
			if (!entity)
				return;

			Vector end = Vector(game_event->GetFloat("x"), game_event->GetFloat("y"), game_event->GetFloat("z"));
			Vector start = UTILS::TraceToEnd(end, entity->GetVecOrigin() + entity->GetViewOffset());

			if (SETTINGS::main_configs.bullet_tracers_enabled)
				FEATURES::MISC::bullet_tracers.AddTracer(FEATURES::MISC::BulletTracers::BulletTracer(SETTINGS::main_configs.bullet_tracers_color, start, end));
		}

		void WeaponFireListener::FireGameEvent(SDK::IGameEvent* game_event)
		{
			if (!game_event)
				return;

			FEATURES::RAGEBOT::resolver.EventCallback(game_event);
		}
	}
}
