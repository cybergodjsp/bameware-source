#pragma once
#include "../SDK/CTrace.h"

namespace SDK
{
	class CBaseEntity;
	class ITraceFilter;

	struct Weapon_Info;
}

namespace FEATURES
{
	namespace RAGEBOT
	{
		class Autowall
		{
		public:
			struct Autowall_Return_Info
			{
				int damage;
				int hitgroup;
				int penetration_count;
				bool did_penetrate_wall;
				float thickness;
				Vector end;
				SDK::CBaseEntity* hit_entity;
			};

			Autowall_Return_Info CalculateDamage(Vector start, Vector end, SDK::CBaseEntity* from_entity = nullptr, SDK::CBaseEntity* to_entity = nullptr, int specific_hitgroup = -1);
			float GetThickness(Vector start, Vector end);

			inline bool IsAutowalling() const
			{
				return is_autowalling;
			}

		private:
			bool is_autowalling = false;

			struct Autowall_Info
			{
				Vector start;
				Vector end;
				Vector current_position;
				Vector direction;

				SDK::ITraceFilter* filter;
				SDK::trace_t enter_trace;

				float thickness;
				float current_damage;
				int penetration_count;
			};

			bool CanPenetrate(SDK::CBaseEntity* attacker, Autowall_Info& info, SDK::Weapon_Info* weapon_data);

			void ScaleDamage(SDK::CBaseEntity* entity, SDK::Weapon_Info* weapon_info, int hitgroup, float& current_damage);

			void UTIL_TraceLine(Vector& absStart, Vector& absEnd, unsigned int mask, SDK::CBaseEntity* ignore, SDK::trace_t* ptr)
			{
				SDK::Ray_t ray;
				ray.Init(absStart, absEnd);
				SDK::CTraceFilter filter;
				filter.pSkip1 = ignore;

				INTERFACES::Trace->TraceRay(ray, mask, &filter, ptr);
			}

			void UTIL_ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, SDK::ITraceFilter* filter, SDK::trace_t* tr)
			{
				static DWORD dwAddress = UTILS::FindPattern("client.dll", (BYTE*)"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x43\x10", "xxxxxxxxxxxxxxxxxxxxxxxx????xxx");
				if (!dwAddress)
					return;

				_asm
				{
					MOV EAX, filter
					LEA ECX, tr
					PUSH ECX
					PUSH EAX
					PUSH mask
					LEA EDX, vecAbsEnd
					LEA ECX, vecAbsStart
					CALL dwAddress
					ADD ESP, 0xC
				}
			}

			void GetBulletTypeParameters(float& maxRange, float& maxDistance)
			{
				maxRange = 35.0;
				maxDistance = 3000.0;
			}

			bool IsBreakableEntity(SDK::CBaseEntity* entity)
			{
				if (!entity || !entity->GetIndex())
					return false;

				//m_takeDamage isn't properly set when using the signature.
				//Back it up, set it to true, then restore.
				int takeDamageBackup = *reinterpret_cast<byte*>(uintptr_t(entity) + 0x27C);

				auto pClass = entity->GetClientClass();
				if (!pClass)
					return false;

				//				 '       ''     '      '   '
				//			    01234567890123456     012345678
				//Check against CBreakableSurface and CBaseDoor

				//Windows etc. are CBrekableSurface
				//Large garage door in Office is CBaseDoor and it get's reported as a breakable when it is not one
				//This is seperate from "CPropDoorRotating", which is a normal door.
				//Normally you would also check for "CFuncBrush" but it was acting oddly so I removed it. It's below if interested.
				//((clientClass->m_pNetworkName[1]) != 'F' || (clientClass->m_pNetworkName[4]) != 'c' || (clientClass->m_pNetworkName[5]) != 'B' || (clientClass->m_pNetworkName[9]) != 'h')

				if ((pClass->m_pNetworkName[1] == 'B' && pClass->m_pNetworkName[9] == 'e' && pClass->m_pNetworkName[10] == 'S' && pClass->m_pNetworkName[16] == 'e')
					|| (pClass->m_pNetworkName[1] != 'B' || pClass->m_pNetworkName[5] != 'D'))
					*reinterpret_cast<byte*>(uintptr_t(entity) + 0x27C) = 2;

				typedef bool (__thiscall* IsBreakbaleEntity_Fn)(SDK::CBaseEntity*);
				static IsBreakbaleEntity_Fn is_breakable_entity = (IsBreakbaleEntity_Fn)UTILS::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x51\x56\x8B\xF1\x85\xF6\x74\x68", "xxxxxxxxxxx");

				bool breakable = is_breakable_entity(entity);
				*reinterpret_cast<byte*>(uintptr_t(entity) + 0x27C) = takeDamageBackup;

				return breakable;
			}

			bool TraceToExit(SDK::trace_t& enterTrace, SDK::trace_t& exitTrace, Vector startPosition, Vector direction);
			bool HandleBulletPenetration(SDK::Weapon_Info* weaponData, SDK::trace_t& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration, float& current_thickness);
		};

		extern Autowall autowall;
	}
}
