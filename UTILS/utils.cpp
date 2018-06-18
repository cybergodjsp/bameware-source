#include "../includes.h"

#include "interfaces.h"

#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CTrace.h"
#include "../SDK/ConVar.h"

#include "../SDK/CBaseEntity.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CUserCmd.h"

#include "utils.h"

namespace UTILS
{
	float GetCurtime()
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player)
			return 0.f;

		return local_player->GetTickBase() * INTERFACES::Globals->interval_per_tick;
	}

	float GetLBYRotatedYaw(float lby, float yaw)
	{
		float delta = MATH::NormalizeYaw(yaw - lby);
		if (fabs(delta) < 25.f)
			return lby;

		if (delta > 0.f)
			return yaw + 25.f;

		return yaw;
	}

	float GetLatency()
	{
		auto nci = INTERFACES::Engine->GetNetChannelInfo();
		if (!nci)
			return 0.f;

		return nci->GetLatency(FLOW_OUTGOING);
	}

	unsigned int FindSignature(const char* module_name, const char* signature)
	{
		MODULEINFO modInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module_name), &modInfo, sizeof(MODULEINFO));

		/// get beggining and end of the dll
		unsigned int start_address = reinterpret_cast<unsigned int>(modInfo.lpBaseOfDll),
			end_address = start_address + modInfo.SizeOfImage;

		const char* pattern = signature;
		unsigned int first_match = 0;
		for (unsigned int current_address = start_address; current_address < end_address; current_address++)
		{
			if (!*pattern)
				return first_match;

			if (*(unsigned char*)pattern == '\?' || *(unsigned char*)current_address == getByte(pattern))
			{
				if (!first_match)
					first_match = current_address;

				if (!pattern[2])
					return first_match;

				if (*(unsigned short*)pattern == '\?\ ? ' || *(unsigned char*)pattern != '\ ? ')
					                                        pattern += 3;
					else
						pattern += 2;
			}
			else
			{
				pattern = signature;
				first_match = 0;
			}
		}

		return 0;
	}

	unsigned int FindPattern(const char* module_name, const BYTE* mask, const char* mask_string)
	{
		/// Get module address
		const unsigned int module_address = reinterpret_cast<unsigned int>(GetModuleHandle(module_name));

		/// Get module information to the size
		MODULEINFO module_info;
		GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(module_address), &module_info, sizeof(MODULEINFO));

		auto IsCorrectMask = [](const unsigned char* data, const unsigned char* mask, const char* mask_string) -> bool
			{
				for (; *mask_string; ++mask_string , ++mask , ++data)
					if (*mask_string == 'x' && *mask != *data)
						return false;

				return (*mask_string) == 0;
			};

		/// Iterate until we find a matching mask
		for (unsigned int c = 0; c < module_info.SizeOfImage; c += 1)
		{
			/// does it match?
			if (IsCorrectMask(reinterpret_cast<unsigned char*>(module_address + c), mask, mask_string))
				return (module_address + c);
		}

		return 0;
	}

	const char* GetPlayerName(SDK::CBaseEntity* player)
	{
		SDK::player_info_t player_info;
		if (!INTERFACES::Engine->GetPlayerInfo(player->GetIndex(), &player_info))
			return "";

		return player_info.name;
	}

	const char* GetHitgroupName(int hitgroup)
	{
		switch (hitgroup)
		{
			case 1:
				return "Head";
			case 2:
				return "Chest";
			case 3:
				return "Pelvis";
			case 4:
				return "Left Arm";
			case 5:
				return "Right Arm";
			case 6:
				return "Left Leg";
			case 7:
				return "Right Leg";
			default:
				return "";
		}
	}

	bool CanShoot(bool right_click)
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player || local_player->GetHealth() <= 0 || local_player->GetNextAttack() > GetCurtime())
			return false;

		auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
		if (!weapon)
			return false;

		/// special shit for nades
		if (IsWeaponGrenade(weapon))
		{
			if (weapon->IsPinPulled())
				return false;

			if (weapon->GetThrowTime() <= 0 || weapon->GetThrowTime() > GetCurtime())
				return false;

			return true;
		}

		if (right_click)
		{
			if (weapon->GetNextSecondaryAttack() > GetCurtime())
				return false;
		}
		else
		{
			if (weapon->GetNextPrimaryAttack() > GetCurtime())
				return false;

			/// revolver
			if (weapon->GetItemDefinitionIndex() == SDK::Weapon_Class_ID::WEAPON_REVOLVER && weapon->GetPostponeFireReadyTime() > GetCurtime())
				return false;
		}

		return true;
	}

	bool IsWeaponGrenade(SDK::CBaseWeapon* weapon)
	{
		if (!weapon)
			return false;

		auto weapon_info = weapon->GetWeaponInfo();
		if (!weapon_info)
			return false;

		if (weapon_info->m_WeaponType == WEAPON_TYPE_GRENADE)
			return true;

		return false;
	}

	bool IsWeaponSniper(SDK::CBaseWeapon* weapon)
	{
		if (!weapon)
			return false;

		auto weapon_info = weapon->GetWeaponInfo();
		if (!weapon_info)
			return false;

		if (weapon_info->m_WeaponType == WEAPON_TYPE_SNIPER)
			return true;

		return false;
	}

	bool IsWeaponKnife(SDK::CBaseWeapon* weapon)
	{
		if (!weapon)
			return false;

		auto weapon_info = weapon->GetWeaponInfo();
		if (!weapon_info)
			return false;

		if (weapon_info->m_WeaponType == WEAPON_TYPE_KNIFE && !IsWeaponTaser(weapon))
			return true;

		return false;
	}

	bool IsWeaponTaser(SDK::CBaseWeapon* weapon)
	{
		if (!weapon)
			return false;

		if (weapon->GetItemDefinitionIndex() == SDK::Weapon_Class_ID::WEAPON_ZEUS)
			return true;

		return false;
	}

	bool IsPressingMovementKeys(SDK::CUserCmd* cmd)
	{
		if (!cmd)
			return false;

		if (cmd->buttons & IN_FORWARD ||
			cmd->buttons & IN_BACK || cmd->buttons & IN_LEFT || cmd->buttons & IN_RIGHT ||
			cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT)
			return true;

		return false;
	}

	void RotateMovement(SDK::CUserCmd* cmd, float yaw)
	{
		Vector viewangles;
		INTERFACES::Engine->GetViewAngles(viewangles);

		float rotation = DEG2RAD(viewangles.y - yaw);

		float cos_rot = cos(rotation);
		float sin_rot = sin(rotation);

		float new_forwardmove = (cos_rot * cmd->forwardmove) - (sin_rot * cmd->sidemove);
		float new_sidemove = (sin_rot * cmd->forwardmove) + (cos_rot * cmd->sidemove);

		cmd->forwardmove = new_forwardmove;
		cmd->sidemove = new_sidemove;
	}

	int TicksToStop(Vector velocity)
	{
		static auto sv_maxspeed = INTERFACES::cvar->FindVar(enc_char("sv_maxspeed"));

		const float max_speed = 320.f;
		const float acceleration = 5.5f;
		const float max_accelspeed = acceleration * max_speed * INTERFACES::Globals->interval_per_tick;

		return velocity.Length() / max_accelspeed;
	}

	SDK::CBaseWeapon* GetWeapon(SDK::CBaseEntity* entity)
	{
		return reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));
	}

	Vector TraceToEnd(Vector start, Vector end)
	{
		SDK::trace_t trace;
		SDK::CTraceWorldOnly filter;
		SDK::Ray_t ray;

		ray.Init(start, end);
		INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

		return trace.end;
	}
}
