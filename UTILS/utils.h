#pragma once

namespace SDK
{
	class CBaseEntity;
	class CUserCmd;
	class CBaseWeapon;
}

namespace UTILS
{
	template <class T>
	constexpr const T& Clamp(const T& v, const T& lo, const T& hi)
	{
		return (v >= lo && v <= hi) ? v : (v < lo ? lo : hi);
	}

	template <class T>
	constexpr const T& Min(const T& x, const T& y)
	{
		return (x > y) ? y : x;
	}

	template <class T>
	constexpr const T& Max(const T& x, const T& y)
	{
		return (x < y) ? y : x;
	}

	constexpr unsigned int FNVHashEx(const char* const data, const unsigned int value = 2166136261)
	{
		return (data[0] == '\0') ? value : (FNVHashEx(&data[1], (value * 16777619) ^ data[0]));
	}

	constexpr unsigned int FNVHash(const void* const data, const int len)
	{
		unsigned char* p = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(data));
		unsigned int h = 2166136261;

		for (int i = 0; i < len; i++)
			h = (h * 16777619) ^ p[i];

		return h;
	}

	float GetCurtime();
	float GetLBYRotatedYaw(float lby, float yaw);
	float GetLatency();

	unsigned int FindSignature(const char* module_name, const char* signature);
	unsigned int FindPattern(const char* module_name, const BYTE* mask, const char* mask_string);

	const char* GetPlayerName(SDK::CBaseEntity* player);
	const char* GetHitgroupName(int hitgroup);

	bool CanShoot(bool right_click = false);
	bool IsWeaponGrenade(SDK::CBaseWeapon* weapon);
	bool IsWeaponSniper(SDK::CBaseWeapon* weapon);
	bool IsWeaponKnife(SDK::CBaseWeapon* weapon);
	bool IsWeaponTaser(SDK::CBaseWeapon* weapon);

	bool IsPressingMovementKeys(SDK::CUserCmd* cmd);

	void RotateMovement(SDK::CUserCmd* cmd, float yaw);

	int TicksToStop(Vector velocity);

	SDK::CBaseWeapon* GetWeapon(SDK::CBaseEntity* entity);

	Vector TraceToEnd(Vector start, Vector end);
}
