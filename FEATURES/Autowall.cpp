#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../UTILS/render.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/IEngine.h"
#include "../SDK/SurfaceData.h"
#include "../SDK/ConVar.h"

#include "Autowall.h"

namespace FEATURES
{
	namespace RAGEBOT
	{
		Autowall autowall;

		Autowall::Autowall_Return_Info Autowall::CalculateDamage(Vector start, Vector end, SDK::CBaseEntity* from_entity, SDK::CBaseEntity* to_entity, int specific_hitgroup)
		{
			// default values for return info, in case we need to return abruptly
			Autowall_Return_Info return_info;
			return_info.damage = -1;
			return_info.hitgroup = -1;
			return_info.hit_entity = nullptr;
			return_info.penetration_count = 4;
			return_info.thickness = 0.f;
			return_info.did_penetrate_wall = false;

			Autowall_Info autowall_info;
			autowall_info.penetration_count = 4;
			autowall_info.start = start;
			autowall_info.end = end;
			autowall_info.current_position = start;
			autowall_info.thickness = 0.f;

			// direction 
			MATH::AngleVectors(MATH::CalcAngle(start, end), &autowall_info.direction);

			// attacking entity
			if (!from_entity)
				from_entity = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!from_entity)
				return return_info;

			auto filter_player = SDK::CTraceFilterOneEntity();
			filter_player.pEntity = to_entity;
			auto filter_local = SDK::CTraceFilter();
			filter_local.pSkip1 = from_entity;

			// setup filters
			if (to_entity)
				autowall_info.filter = &filter_player;
			else
				autowall_info.filter = &filter_player;

			// weapon
			auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(from_entity->GetActiveWeaponIndex()));
			if (!weapon)
				return return_info;

			// weapon data
			auto weapon_info = weapon->GetWeaponInfo();
			if (!weapon_info)
				return return_info;

			// client class
			auto weapon_client_class = reinterpret_cast<SDK::CBaseEntity*>(weapon)->GetClientClass();
			if (!weapon_client_class)
				return return_info;

			// weapon range
			float range = min(weapon_info->m_Range, (start - end).Length());
			end = start + (autowall_info.direction * range);
			autowall_info.current_damage = weapon_info->m_Damage;

			while (autowall_info.current_damage > 0 && autowall_info.penetration_count > 0)
			{
				return_info.penetration_count = autowall_info.penetration_count;

				UTIL_TraceLine(autowall_info.current_position, end, MASK_SHOT | CONTENTS_GRATE, from_entity, &autowall_info.enter_trace);
				UTIL_ClipTraceToPlayers(autowall_info.current_position, autowall_info.current_position + autowall_info.direction * 40.f, MASK_SHOT | CONTENTS_GRATE, autowall_info.filter, &autowall_info.enter_trace);

				const float distance_traced = (autowall_info.enter_trace.end - start).Length();
				autowall_info.current_damage *= pow(weapon_info->m_RangeModifier, (distance_traced / 500.f));

				/// if reached the end
				if (autowall_info.enter_trace.flFraction == 1.f)
				{
					if (to_entity && specific_hitgroup != -1)
					{
						ScaleDamage(to_entity, weapon_info, specific_hitgroup, autowall_info.current_damage);

						return_info.damage = autowall_info.current_damage;
						return_info.hitgroup = specific_hitgroup;
						return_info.end = autowall_info.enter_trace.end;
						return_info.hit_entity = to_entity;
					}
					else
					{
						return_info.damage = autowall_info.current_damage;
						return_info.hitgroup = -1;
						return_info.end = autowall_info.enter_trace.end;
						return_info.hit_entity = nullptr;
					}

					break;
				}
				// if hit an entity
				if (autowall_info.enter_trace.hitGroup > 0 && autowall_info.enter_trace.hitGroup <= 7 && autowall_info.enter_trace.m_pEnt)
				{
					// checkles gg
					if ((to_entity && autowall_info.enter_trace.m_pEnt != to_entity) ||
						(autowall_info.enter_trace.m_pEnt->GetTeam() == from_entity->GetTeam()))
					{
						return_info.damage = -1;
						return return_info;
					}

					if (specific_hitgroup != -1)
						ScaleDamage(autowall_info.enter_trace.m_pEnt, weapon_info, specific_hitgroup, autowall_info.current_damage);
					else
						ScaleDamage(autowall_info.enter_trace.m_pEnt, weapon_info, autowall_info.enter_trace.hitGroup, autowall_info.current_damage);

					// fill the return info
					return_info.damage = autowall_info.current_damage;
					return_info.hitgroup = autowall_info.enter_trace.hitGroup;
					return_info.end = autowall_info.enter_trace.end;
					return_info.hit_entity = autowall_info.enter_trace.m_pEnt;

					break;
				}

				// break out of the loop retard
				if (!CanPenetrate(from_entity, autowall_info, weapon_info))
					break;

				return_info.did_penetrate_wall = true;
			}

			return_info.penetration_count = autowall_info.penetration_count;

			return return_info;
		}

		float Autowall::GetThickness(Vector start, Vector end)
		{
			float current_thickness = 0.f;

			Vector direction;
			MATH::AngleVectors(MATH::CalcAngle(start, end), &direction);

			SDK::CTraceWorldOnly filter;
			SDK::trace_t enter_trace;
			SDK::trace_t exit_trace;
			SDK::Ray_t ray;

			int pen = 0;
			while (pen < 4)
			{
				ray.Init(start, end);

				INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &enter_trace);
				if (enter_trace.flFraction == 1.f)
					return current_thickness;

				start = enter_trace.end;
				if (!TraceToExit(enter_trace, exit_trace, start, direction))
					return current_thickness + 90.f;

				start = exit_trace.end;
				current_thickness += (start - exit_trace.end).Length();

				pen++;
			}

			return current_thickness;
		}

		bool Autowall::CanPenetrate(SDK::CBaseEntity* attacker, Autowall_Info& info, SDK::Weapon_Info* weapon_data)
		{
			//typedef bool(__thiscall* HandleBulletPenetrationFn)(SDK::CBaseEntity*, float&, int&, int*, SDK::trace_t*, Vector*, float, float, float, int, int, float, int*, Vector*, float, float, float*);
			//CBaseEntity *pLocalPlayer, float *flPenetration, int *SurfaceMaterial, char *IsSolid, trace_t *ray, Vector *vecDir, int unused, float flPenetrationModifier, float flDamageModifier, int unused2, int weaponmask, float flPenetration2, int *hitsleft, Vector *ResultPos, int unused3, int unused4, float *damage)
			typedef bool (__thiscall* HandleBulletPenetrationFn)(SDK::CBaseEntity*, float*, int*, char*, SDK::trace_t*, Vector*, int, float, float, int, int, float, int*, Vector*, int, int, float*);
			static HandleBulletPenetrationFn HBP = reinterpret_cast<HandleBulletPenetrationFn>(UTILS::FindPattern("client.dll",
			                                                                                                      (PBYTE)"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF8\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x83\xEC\x78\x8B\x53\x14",
			                                                                                                      "xxxxxxxxxxxxxxxxxxxxxxxxxxxx"));

			auto enter_surface_data = INTERFACES::PhysicsProps->GetSurfaceData(info.enter_trace.surface.surfaceProps);
			if (!enter_surface_data)
				return true;

			char is_solid = 0;
			int material = enter_surface_data->game.material;
			int mask = UTILS::IsWeaponTaser(UTILS::GetWeapon(attacker)) ? 0x1100 : 0x1002;

			// glass and shit gg
			if (info.enter_trace.m_pEnt && !strcmp(enc_char("CBreakableSurface"),
			                                                                   info.enter_trace.m_pEnt->GetClientClass()->m_pNetworkName))
				*reinterpret_cast<byte*>(uintptr_t(info.enter_trace.m_pEnt + 0x27C)) = 2;

			is_autowalling = true;
			bool return_value = !HBP(attacker, &weapon_data->m_Penetration, &material, &is_solid, &info.enter_trace, &info.direction, 0, enter_surface_data->game.flPenetrationModifier, enter_surface_data->game.flDamageModifier, 0, mask, weapon_data->m_Penetration, &info.penetration_count, &info.current_position, 0, 0, &info.current_damage);
			is_autowalling = false;
			return return_value;
		}

		void Autowall::ScaleDamage(SDK::CBaseEntity* entity, SDK::Weapon_Info* weapon_info, int hitgroup, float& current_damage)
		{
			//Cred. to N0xius for reversing this.
			//TODO: _xAE^; look into reversing this yourself sometime

			bool hasHeavyArmor = false;
			int armorValue = entity->GetArmor();

			//Fuck making a new function, lambda beste. ~ Does the person have armor on for the hitbox checked?
			auto IsArmored = [&entity, &hitgroup]()-> bool
				{
					SDK::CBaseEntity* targetEntity = entity;
					switch (hitgroup)
					{
						case HITGROUP_HEAD:
							return targetEntity->HasHelmet();
						case HITGROUP_GENERIC:
						case HITGROUP_CHEST:
						case HITGROUP_STOMACH:
						case HITGROUP_LEFTARM:
						case HITGROUP_RIGHTARM:
							return true;
						default:
							return false;
					}
				};

			switch (hitgroup)
			{
				case HITGROUP_HEAD:
					current_damage *= hasHeavyArmor ? 2.f : 4.f; //Heavy Armor does 1/2 damage
					break;
				case HITGROUP_STOMACH:
					current_damage *= 1.25f;
					break;
				case HITGROUP_LEFTLEG:
				case HITGROUP_RIGHTLEG:
					current_damage *= 0.75f;
					break;
				default:
					break;
			}

			if (armorValue > 0 && IsArmored())
			{
				float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weapon_info->m_ArmorRatio / 2.f;

				//Damage gets modified for heavy armor users
				if (hasHeavyArmor)
				{
					armorBonusRatio = 0.33f;
					armorRatio *= 0.5f;
					bonusValue = 0.33f;
				}

				auto NewDamage = current_damage * armorRatio;

				if (hasHeavyArmor)
					NewDamage *= 0.85f;

				if (((current_damage - (current_damage * armorRatio)) * (bonusValue * armorBonusRatio)) > armorValue)
					NewDamage = current_damage - (armorValue / armorBonusRatio);

				current_damage = NewDamage;
			}
		}

		bool Autowall::TraceToExit(SDK::trace_t& enterTrace, SDK::trace_t& exitTrace, Vector startPosition, Vector direction)
		{
			/*
			Masks used:
			MASK_SHOT_HULL					 = 0x600400B
			CONTENTS_HITBOX					 = 0x40000000
			MASK_SHOT_HULL | CONTENTS_HITBOX = 0x4600400B
			*/

			Vector start, end;
			float maxDistance = 90.f, rayExtension = 4.f, currentDistance = 0;
			int firstContents = 0;

			while (currentDistance <= maxDistance)
			{
				//Add extra distance to our ray
				currentDistance += rayExtension;

				//Multiply the direction vector to the distance so we go outwards, add our position to it.
				start = startPosition + direction * currentDistance;

				if (!firstContents)
					firstContents = INTERFACES::Trace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr); /*0x4600400B*/
				int pointContents = INTERFACES::Trace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

				if (!(pointContents & MASK_SHOT_HULL) || pointContents & CONTENTS_HITBOX && pointContents != firstContents) /*0x600400B, *0x40000000*/
				{
					//Let's setup our end position by deducting the direction by the extra added distance
					end = start - (direction * rayExtension);

					//Let's cast a ray from our start pos to the end pos
					UTIL_TraceLine(start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace);

					//Let's check if a hitbox is in-front of our enemy and if they are behind of a solid wall
					if (exitTrace.startSolid && exitTrace.surface.flags & SURF_HITBOX)
					{
						UTIL_TraceLine(start, startPosition, MASK_SHOT_HULL, exitTrace.m_pEnt, &exitTrace);

						if (exitTrace.DidHit() && !exitTrace.startSolid)
						{
							start = exitTrace.end;
							return true;
						}

						continue;
					}

					//Can we hit? Is the wall solid?
					if (exitTrace.DidHit() && !exitTrace.startSolid)
					{
						//Is the wall a breakable? If so, let's shoot through it.
						if (Autowall::IsBreakableEntity(enterTrace.m_pEnt) && Autowall::IsBreakableEntity(exitTrace.m_pEnt))
							return true;

						if (enterTrace.surface.flags & SURF_NODRAW || !(exitTrace.surface.flags & SURF_NODRAW) && (exitTrace.plane.normal.Dot(direction) <= 1.f))
						{
							float multAmount = exitTrace.flFraction * 4.f;
							start -= direction * multAmount;
							return true;
						}

						continue;
					}

					if (!exitTrace.DidHit() || exitTrace.startSolid)
					{
						if (enterTrace.DidHitNonWorldEntity() && Autowall::IsBreakableEntity(enterTrace.m_pEnt))
						{
							exitTrace = enterTrace;
							exitTrace.end = start + direction;
							return true;
						}
					}
				}
			}
			return false;
		}

		bool Autowall::HandleBulletPenetration(SDK::Weapon_Info* weaponData, SDK::trace_t& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration, float& current_thickness)
		{
			//Because there's been issues regarding this- putting this here.
			if (&currentDamage == nullptr)
				throw std::invalid_argument("currentDamage is null!");

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return false;

			SDK::trace_t exitTrace;
			SDK::CBaseEntity* pEnemy = enterTrace.m_pEnt;
			SDK::surfacedata_t* enterSurfaceData = INTERFACES::PhysicsProps->GetSurfaceData(enterTrace.surface.surfaceProps);
			int enterMaterial = enterSurfaceData->game.material;

			float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;
			float enterDamageModifier = enterSurfaceData->game.flDamageModifier;
			float thickness, modifier, lostDamage, finalDamageModifier, combinedPenetrationModifier;
			bool isSolidSurf = ((enterTrace.contents >> 3) & CONTENTS_SOLID);
			bool isLightSurf = ((enterTrace.surface.flags >> 7) & SURF_LIGHT);

			if (possibleHitsRemaining <= 0
				|| (!possibleHitsRemaining && !isLightSurf && !isSolidSurf && enterMaterial != CHAR_TEX_GRATE && enterMaterial != CHAR_TEX_GLASS)
				|| weaponData->m_Penetration <= 0.f
				|| !TraceToExit(enterTrace, exitTrace, enterTrace.end, direction)
				&& !(INTERFACES::Trace->GetPointContents(enterTrace.end, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL))
				return false;

			SDK::surfacedata_t* exitSurfaceData = INTERFACES::PhysicsProps->GetSurfaceData(exitTrace.surface.surfaceProps);
			int exitMaterial = exitSurfaceData->game.material;
			float exitSurfPenetrationModifier = exitSurfaceData->game.flPenetrationModifier;
			float exitDamageModifier = exitSurfaceData->game.flDamageModifier;

			//Are we using the newer penetration system?
			if (sv_penetration_type)
			{
				if (enterMaterial == CHAR_TEX_GRATE || enterMaterial == CHAR_TEX_GLASS)
				{
					combinedPenetrationModifier = 3.f;
					finalDamageModifier = 0.05f;
				}
				else if (isSolidSurf || isLightSurf)
				{
					combinedPenetrationModifier = 1.f;
					finalDamageModifier = 0.16f;
				}
				else if (enterMaterial == CHAR_TEX_FLESH && (local_player->GetTeam() == pEnemy->GetTeam() && ff_damage_reduction_bullets == 0.f)) //TODO: Team check config
				{
					//Look's like you aren't shooting through your teammate today
					if (ff_damage_bullet_penetration == 0.f)
						return false;

					//Let's shoot through teammates and get kicked for teamdmg! Whatever, atleast we did damage to the enemy. I call that a win.
					combinedPenetrationModifier = ff_damage_bullet_penetration;
					finalDamageModifier = 0.16f;
				}
				else
				{
					combinedPenetrationModifier = (enterSurfPenetrationModifier + exitSurfPenetrationModifier) / 2.f;
					finalDamageModifier = 0.16f;
				}

				//Do our materials line up?
				if (enterMaterial == exitMaterial)
				{
					if (exitMaterial == CHAR_TEX_CARDBOARD || exitMaterial == CHAR_TEX_WOOD)
						combinedPenetrationModifier = 3.f;
					else if (exitMaterial == CHAR_TEX_PLASTIC)
						combinedPenetrationModifier = 2.f;
				}

				//Calculate thickness of the wall by getting the length of the range of the trace and squaring
				thickness = (exitTrace.end - enterTrace.end).LengthSqr();
				modifier = fmaxf(1.f / combinedPenetrationModifier, 0.f);
				current_thickness += thickness;

				//This calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
				lostDamage = fmaxf(
					((modifier * thickness) / 24.f)
					+ ((currentDamage * finalDamageModifier)
						+ (fmaxf(3.75f / penetrationPower, 0.f) * 3.f * modifier)), 0.f);

				//Did we loose too much damage?
				if (lostDamage > currentDamage)
					return false;

				//We can't use any of the damage that we've lost
				if (lostDamage > 0.f)
					currentDamage -= lostDamage;

				//Do we still have enough damage to deal?
				if (currentDamage < 1.f)
					return false;

				eyePosition = exitTrace.end;
				--possibleHitsRemaining;

				return true;
			}
			else //Legacy penetration system
			{
				combinedPenetrationModifier = 1.f;

				if (isSolidSurf || isLightSurf)
					finalDamageModifier = 0.99f; //Good meme :^)
				else
				{
					finalDamageModifier = fminf(enterDamageModifier, exitDamageModifier);
					combinedPenetrationModifier = fminf(enterSurfPenetrationModifier, exitSurfPenetrationModifier);
				}

				if (enterMaterial == exitMaterial && (exitMaterial == CHAR_TEX_METAL || exitMaterial == CHAR_TEX_WOOD))
					combinedPenetrationModifier += combinedPenetrationModifier;

				thickness = (exitTrace.end - enterTrace.end).LengthSqr();

				if (sqrt(thickness) <= combinedPenetrationModifier * penetrationPower)
				{
					currentDamage *= finalDamageModifier;
					eyePosition = exitTrace.end;
					--possibleHitsRemaining;

					return true;
				}

				return false;
			}
		}
	}
}
