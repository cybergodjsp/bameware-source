#pragma once

#include "../UTILS/offsets.h"
#include "../UTILS/interfaces.h"

namespace SDK
{
	struct Weapon_Info
	{
		char pad00[0xC8];
		int m_WeaponType; // 0xC8
		char padCC[0x20];
		int m_Damage; // 0xEC
		float m_ArmorRatio; // 0xF0
		char padF4[0x4];
		float m_Penetration; // 0xF8
		char padFC[0x8];
		float m_Range; // 0x104
		float m_RangeModifier; // 0x108
		char pad10C[0x10];
		bool m_HasSilencer; // 0x11C
	};

	enum Weapon_Class_ID
	{
		WEAPON_REVOLVER = 64,
		WEAPON_KNIFE = 93,
		WEAPON_KNIFE2 = 59,
		WEAPON_ZEUS = 31,
		WEAPON_G3SG1 = 11,
		WEAPON_GLOCK = 4,
		WEAPON_SCOUT = 40,
		WEAPON_AK47 = 7,
		WEAPON_AWP = 9,
		WEAPON_DEAGLE = 1,
		WEAPON_DUELIES = 2,
		WEAPON_P250 = 36,
		WEAPON_TEC9 = 30,
		WEAPON_SCAR20 = 38,
		WEAPON_NEGEV = 28,
		WEAPON_P90 = 19,
		WEAPON_PPBIZON = 26,
		WEAPON_MOLOTOV = 46,
		WEAPON_HEGRENADE = 44,
		WEAPON_SMOKE = 45,
		WEAPON_UMP = 24,
		WEAPON_MP7 = 33,
		WEAPON_AUG = 8,
		WEAPON_FIVESEVEN = 3,
		WEAPON_MAC10 = 17,
		WEAPON_FAMAS = 10,
		WEAPON_GALIL = 13,
		WEAPON_M4A4 = 16,
		WEAPON_M249 = 14,
		WEAPON_XM1014 = 25,
		WEAPON_C4 = 49,
		WEAPON_P2000 = 32
	};

	enum ItemDefinitionIndex
	{
		weapon_none,
		weapon_deagle,
		weapon_elite,
		weapon_fiveseven,
		weapon_glock,
		weapon_ak47 = 7,
		weapon_aug,
		weapon_awp,
		weapon_famas,
		weapon_g3sg1,
		weapon_galilar = 13,
		weapon_m249,
		weapon_m4a4 = 16,
		weapon_mac10,
		weapon_p90 = 19,
		weapon_ump45 = 24,
		weapon_xm1014,
		weapon_bizon,
		weapon_mag7,
		weapon_negev,
		weapon_sawedoff,
		weapon_tec9,
		weapon_taser,
		weapon_p2000,
		weapon_mp7,
		weapon_mp9,
		weapon_nova,
		weapon_p250,
		weapon_scar20 = 38,
		weapon_sg556,
		weapon_ssg08,
		weapon_knife_ct = 42,
		weapon_flashbang,
		weapon_hegrenade,
		weapon_smokegrenade,
		weapon_molotov,
		weapon_decoy,
		weapon_incgrenade,
		weapon_c4,
		weapon_knife_t = 59,
		weapon_m4a1s = 60,
		weapon_usp = 61,
		weapon_cz75 = 63,
		weapon_revolver,
		weapon_tagrenade = 68,
		weapon_bayonet = 500,
		weapon_flip = 505,
		weapon_gut,
		weapon_karambit,
		weapon_m9bayonet,
		weapon_huntsman,
		weapon_falchion = 512,
		weapon_bowie = 514,
		weapon_butterfly = 515,
		weapon_pushdagger,
		studded_bloodhound_gloves = 5027,
		sporty_gloves = 5030,
		slick_gloves = 5031,
		leather_handwraps = 5032,
		motorcycle_gloves = 5033,
		specialist_gloves = 5034
	};

	class CBaseWeapon
	{
	public:

		float GetNextPrimaryAttack()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flNextPrimaryAttack);
		}

		float GetNextSecondaryAttack()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flNextSecondaryAttack);
		}

		float GetSpreadCone()
		{
			typedef float (__thiscall* Fn)(void*);
			return VMT::GetVFunction<Fn>(this, 439)(this);
		}

		float GetInnaccuracy()
		{
			typedef float (__thiscall* Fn)(void*);
			return VMT::GetVFunction<Fn>(this, 470)(this);
		}

		void UpdateAccuracyPenalty()
		{
			typedef void (__thiscall* Fn)(void*);
			return VMT::GetVFunction<Fn>(this, 471)(this);
		}

		Weapon_Info* GetWeaponInfo()
		{
			typedef Weapon_Info*(__thiscall* Fn)(void*);
			return VMT::GetVFunction<Fn>(this, 447)(this);
		}

		int GetItemDefinitionIndex()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_iItemDefinitionIndex);
		}

		void SetItemDefinitionIndex(int value)
		{
			*reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_iItemDefinitionIndex) = value;
		}

		void SetItemIDHigh(int value)
		{
			*reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_iItemIDHigh) = value;
		}

		void SetFallbackPaintKit(int value)
		{
			*reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_nFallbackPaintKit) = value;
		}

		void SetFallbackWear(float value)
		{
			*reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flFallbackWear) = value;
		}

		char* GetCustomName()
		{
			return reinterpret_cast<char*>(uintptr_t(this) + OFFSETS::m_szCustomName);
		}

		void SetFallbackSeed(int value)
		{
			*reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_nFallbackSeed) = value;
		}

		void SetFallbackStatTrak(int value)
		{
			*reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_nFallbackStatTrak) = value;
		}

		void SetEntityQuality(int value)
		{
			*reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_iEntityQuality) = value;
		}

		void SetAccountID(int value)
		{
			*reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_iAccountID) = value;
		}

		float GetPostponeFireReadyTime()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flPostponeFireReadyTime);
		}

		float GetThrowTime()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_fThrowTime);
		}

		bool IsPinPulled()
		{
			return *reinterpret_cast<bool*>(uintptr_t(this) + OFFSETS::m_bPinPulled);
		}

		int GetAmmo()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_iClip1);
		}

		int GetWeaponWorldModelIndex()
		{
			return (*reinterpret_cast<uintptr_t*>(uintptr_t(this) + OFFSETS::m_hWeaponWorldModel)) & 0xFFF;
		}

		void SetViewModelIndex(int value)
		{
			*reinterpret_cast<uintptr_t*>(uintptr_t(this) + OFFSETS::m_iViewModelIndex) = value;
		}

		void SetWorldModelIndex(int value)
		{
			*reinterpret_cast<uintptr_t*>(uintptr_t(this) + OFFSETS::m_iWorldModelIndex) = value;
		}
	};

	class CBaseViewmodel : public CBaseEntity
	{
	public:
		int GetWeaponIndex()
		{
			return (*reinterpret_cast<uintptr_t*>(uintptr_t(this) + OFFSETS::m_hWeapon)) & 0xFFF;
		}

		int GetOwnerIndex()
		{
			return (*reinterpret_cast<uintptr_t*>(uintptr_t(this) + OFFSETS::m_hOwner)) & 0xFFF;
		}

	private:
	};
}
