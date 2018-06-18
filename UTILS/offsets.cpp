#include "../includes.h"

#include "NetvarManager.h"

#include "offsets.h"

namespace OFFSETS
{
	uintptr_t m_iHealth;
	uintptr_t m_fFlags;
	uintptr_t m_vecVelocity;
	uintptr_t m_flLowerBodyYawTarget;
	uintptr_t deadflag;
	uintptr_t m_vecOrigin;
	uintptr_t m_iTeamNum;
	uintptr_t m_nTickBase;
	uintptr_t m_bDormant;
	uintptr_t animstate;
	uintptr_t m_Collision;
	uintptr_t m_angEyeAngles;
	uintptr_t m_flSimulationTime;
	uintptr_t m_vecViewOffset;
	uintptr_t m_dwBoneMatrix;
	uintptr_t m_aimPunchAngle;
	uintptr_t m_bGunGameImmunity;
	uintptr_t m_nForceBone;
	uintptr_t m_flPoseParameter;
	uintptr_t dwGlowObjectManager;
	uintptr_t m_flNextPrimaryAttack;
	uintptr_t m_flNextAttack;
	uintptr_t m_hActiveWeapon;
	uintptr_t m_ArmorValue;
	uintptr_t m_bHasHelmet;
	uintptr_t m_bIsScoped;
	uintptr_t ClientState;
	uintptr_t m_iItemDefinitionIndex;
	uintptr_t m_iItemIDHigh;
	uintptr_t m_nFallbackPaintKit;
	uintptr_t m_flFallbackWear;
	uintptr_t m_szCustomName;
	uintptr_t m_hMyWeapons;
	uintptr_t m_nFallbackSeed;
	uintptr_t m_nFallbackStatTrak;
	uintptr_t m_iEntityQuality;
	uintptr_t m_iAccountID;
	uintptr_t m_flPostponeFireReadyTime;
	uintptr_t m_iObserverMode;
	uintptr_t m_flFlashDuration;
	uintptr_t m_flThrowStrength;
	uintptr_t m_bPinPulled;
	uintptr_t m_bSpotted;
	uintptr_t m_flNextSecondaryAttack;
	uintptr_t m_fThrowTime;
	uintptr_t m_MoveType;
	uintptr_t m_iClip1;
	uintptr_t m_iPrimaryReserveAmmoCount;
	uintptr_t m_hViewModel;
	uintptr_t m_nModelIndex;
	uintptr_t m_hWeapon;
	uintptr_t m_hOwner;
	uintptr_t m_hWeaponWorldModel;
	uintptr_t m_iViewModelIndex;
	uintptr_t m_iWorldModelIndex;
	uintptr_t m_angRotation;

	void InitOffsets()
	{
		m_vecVelocity = 0x110;
		deadflag = 0x31C4;
		m_nTickBase = 0x3404;
		m_bDormant = 0xE9;
		animstate = 0x3874;
		m_vecViewOffset = 0x104;
		m_dwBoneMatrix = 0x2698;
		m_aimPunchAngle = 0x301C;
		dwGlowObjectManager = 0x4F9A128;
		ClientState = 0x57B7EC;
		m_MoveType = 0x258;
		m_hViewModel = 0x32DC;

		uintptr_t m_AttributeManager = UTILS::netvar_manager.GetOffset(enc_char("DT_EconEntity"), enc_char("m_AttributeManager"));
		uintptr_t m_Item = UTILS::netvar_manager.GetOffset(enc_char("DT_AttributeContainer"), enc_char("m_Item"));

		m_nForceBone = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseAnimating"), enc_char("m_nForceBone"));
		m_flPoseParameter = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseAnimating"), enc_char("m_flPoseParameter"));
		m_flNextPrimaryAttack = UTILS::netvar_manager.GetOffset(enc_char("DT_LocalActiveWeaponData"), enc_char("m_flNextPrimaryAttack"));
		m_flNextSecondaryAttack = UTILS::netvar_manager.GetOffset(enc_char("DT_LocalActiveWeaponData"), enc_char("m_flNextSecondaryAttack"));
		m_flNextAttack = UTILS::netvar_manager.GetOffset(enc_char("DT_BCCLocalPlayerExclusive"), enc_char("m_flNextAttack"));
		m_iHealth = UTILS::netvar_manager.GetOffset(enc_char("DT_BasePlayer"), enc_char("m_iHealth"));
		m_fFlags = UTILS::netvar_manager.GetOffset(enc_char("DT_BasePlayer"), enc_char("m_fFlags"));
		m_flLowerBodyYawTarget = UTILS::netvar_manager.GetOffset(enc_char("DT_CSPlayer"), enc_char("m_flLowerBodyYawTarget"));
		m_vecOrigin = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseEntity"), enc_char("m_vecOrigin"));
		m_iTeamNum = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseEntity"), enc_char("m_iTeamNum"));
		m_Collision = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseEntity"), enc_char("m_Collision"));
		m_angEyeAngles = UTILS::netvar_manager.GetOffset(enc_char("DT_CSPlayer"), enc_char("m_angEyeAngles"));
		m_flSimulationTime = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseEntity"), enc_char("m_flSimulationTime"));
		m_bGunGameImmunity = UTILS::netvar_manager.GetOffset(enc_char("DT_CSPlayer"), enc_char("m_bGunGameImmunity"));
		m_hActiveWeapon = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseCombatCharacter"), enc_char("m_hActiveWeapon"));
		m_ArmorValue = UTILS::netvar_manager.GetOffset(enc_char("DT_CSPlayer"), enc_char("m_ArmorValue"));
		m_bHasHelmet = UTILS::netvar_manager.GetOffset(enc_char("DT_CSPlayer"), enc_char("m_bHasHelmet"));
		m_bIsScoped = UTILS::netvar_manager.GetOffset(enc_char("DT_CSPlayer"), enc_char("m_bIsScoped"));
		m_nFallbackPaintKit = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseAttributableItem"), enc_char("m_nFallbackPaintKit"));
		m_flFallbackWear = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseAttributableItem"), enc_char("m_flFallbackWear"));
		m_hMyWeapons = UTILS::netvar_manager.GetOffset(enc_char("DT_BCCNonLocalPlayerExclusive"), enc_char("m_hMyWeapons"));
		m_nFallbackSeed = UTILS::netvar_manager.GetOffset(enc_char("DT_EconEntity"), enc_char("m_nFallbackSeed"));
		m_nFallbackStatTrak = UTILS::netvar_manager.GetOffset(enc_char("DT_EconEntity"), enc_char("m_nFallbackStatTrak"));
		m_iItemDefinitionIndex = m_AttributeManager + m_Item + UTILS::netvar_manager.GetOffset(enc_char("DT_ScriptCreatedItem"), enc_char("m_iItemDefinitionIndex"));
		m_iItemIDHigh = m_AttributeManager + m_Item + UTILS::netvar_manager.GetOffset(enc_char("DT_ScriptCreatedItem"), enc_char("m_iItemIDHigh"));
		m_szCustomName = m_AttributeManager + m_Item + UTILS::netvar_manager.GetOffset(enc_char("DT_ScriptCreatedItem"), enc_char("m_szCustomName"));
		m_iEntityQuality = m_AttributeManager + m_Item + UTILS::netvar_manager.GetOffset(enc_char("DT_ScriptCreatedItem"), enc_char("m_iEntityQuality"));
		m_iAccountID = m_AttributeManager + m_Item + UTILS::netvar_manager.GetOffset(enc_char("DT_ScriptCreatedItem"), enc_char("m_iAccountID"));
		m_flPostponeFireReadyTime = UTILS::netvar_manager.GetOffset(enc_char("DT_WeaponCSBase"), enc_char("m_flPostponeFireReadyTime"));
		m_iObserverMode = UTILS::netvar_manager.GetOffset(enc_char("DT_BasePlayer"), enc_char("m_iObserverMode"));
		m_flFlashDuration = UTILS::netvar_manager.GetOffset(enc_char("DT_CSPlayer"), enc_char("m_flFlashDuration"));
		m_flThrowStrength = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseCSGrenade"), enc_char("m_flThrowStrength"));
		m_bPinPulled = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseCSGrenade"), enc_char("m_bPinPulled"));
		m_bSpotted = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseEntity"), enc_char("m_bSpotted"));
		m_fThrowTime = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseCSGrenade"), enc_char("m_fThrowTime"));
		m_iClip1 = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseCombatWeapon"), enc_char("m_iClip1"));
		m_iPrimaryReserveAmmoCount = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseCombatWeapon"), enc_char("m_iPrimaryReserveAmmoCount"));
		m_nModelIndex = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseEntity"), enc_char("m_nModelIndex"));
		m_hWeapon = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseViewModel"), enc_char("m_hWeapon"));
		m_hOwner = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseViewModel"), enc_char("m_hOwner"));
		m_hWeaponWorldModel = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseCombatWeapon"), enc_char("m_hWeaponWorldModel"));
		m_iViewModelIndex = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseCombatWeapon"), enc_char("m_iViewModelIndex"));
		m_iWorldModelIndex = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseCombatWeapon"), enc_char("m_iWorldModelIndex"));
		m_angRotation = UTILS::netvar_manager.GetOffset(enc_char("DT_BaseEntity"), enc_char("m_angRotation"));
	}
}
