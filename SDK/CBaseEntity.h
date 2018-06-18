#pragma once

#include "../UTILS/offsets.h"
#include "../UTILS/interfaces.h"
#include "../SDK/ModelInfo.h"

#include "AnimLayer.h"
#include "RecvData.h"

namespace SDK
{
	class CBaseAnimState;
	class Collideable;

	struct studiohdr_t;
	struct model_t;

	class CBaseEntity
	{
	public:
		int GetHealth()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_iHealth);
		}

		int GetFlags()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_fFlags);
		}

		void SetFlags(int flags)
		{
			*reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_fFlags) = flags;
		}

		int GetTeam()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_iTeamNum);
		}

		Vector GetVelocity()
		{
			return *reinterpret_cast<Vector*>(uintptr_t(this) + OFFSETS::m_vecVelocity);
		}

		void SetVelocity(Vector velocity)
		{
			*reinterpret_cast<Vector*>(uintptr_t(this) + OFFSETS::m_vecVelocity) = velocity;
		}

		float GetLowerBodyYaw()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flLowerBodyYawTarget);
		}

		Vector GetVecOrigin()
		{
			return *reinterpret_cast<Vector*>(uintptr_t(this) + OFFSETS::m_vecOrigin);
		}

		int GetTickBase()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_nTickBase);
		}

		bool GetIsDormant()
		{
			return *reinterpret_cast<bool*>(uintptr_t(this) + OFFSETS::m_bDormant);
		}

		CBaseAnimState* GetAnimState()
		{
			return *reinterpret_cast<CBaseAnimState**>(uintptr_t(this) + OFFSETS::animstate);
		}

		Collideable* GetCollideable()
		{
			return (Collideable*)((DWORD)this + OFFSETS::m_Collision);
		}

		void GetRenderBounds(Vector& mins, Vector& maxs)
		{
			void* pRenderable = (void*)(this + 0x4);
			typedef void (__thiscall* Fn)(void*, Vector&, Vector&);
			VMT::GetVFunction<Fn>(pRenderable, 17)(pRenderable, mins, maxs);
		}

		int GetIndex()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + 0x64);
		}

		CAnimationLayer& GetAnimOverlay(int Index)
		{
			return (*(CAnimationLayer**)((DWORD)this + 0x2970))[Index];
		}

		void SetAnimOverlay(int Index, CAnimationLayer layer)
		{
			(*(CAnimationLayer**)((DWORD)this + 0x2970))[Index] = layer;
		}

		int CBaseEntity::GetSequenceActivity(int sequence)
		{
			auto model = GetModel();
			if (!model)
				return -1;

			auto hdr = INTERFACES::ModelInfo->GetStudioModel(model);
			if (!hdr)
				return -1;

			// c_csplayer vfunc 242, follow calls to find the function.

			// (DWORD)Utils::PatternScan(client, "55 8B EC 83 7D 08 FF 56 8B F1 74 3D")
			static auto offset = (DWORD)UTILS::FindSignature("client.dll", "55 8B EC 83 7D 08 FF 56 8B F1 74 3D");
			static auto GetSequenceActivity = reinterpret_cast<int(__fastcall*)(void*, SDK::studiohdr_t*, int)>(offset);

			return GetSequenceActivity(this, hdr, sequence);
		}

		Vector GetEyeAngles()
		{
			return *reinterpret_cast<Vector*>(uintptr_t(this) + OFFSETS::m_angEyeAngles);
		}

		void SetEyeAngles(Vector angles)
		{
			*reinterpret_cast<Vector*>(uintptr_t(this) + OFFSETS::m_angEyeAngles) = angles;
		}

		float GetSimTime()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flSimulationTime);
		}

		Vector GetViewOffset()
		{
			return *reinterpret_cast<Vector*>(uintptr_t(this) + OFFSETS::m_vecViewOffset);
		}

		model_t* GetModel()
		{
			void* pRenderable = reinterpret_cast<void*>(uintptr_t(this) + 0x4);
			typedef model_t* (__thiscall* fnGetModel)(void*);

			return VMT::GetVFunction<fnGetModel>(pRenderable, 8)(pRenderable);
		}

		matrix3x4_t GetBoneMatrix(int BoneID)
		{
			matrix3x4_t matrix;

			auto offset = *reinterpret_cast<uintptr_t*>(uintptr_t(this) + OFFSETS::m_dwBoneMatrix);
			if (offset)
				matrix = *reinterpret_cast<matrix3x4_t*>(offset + 0x30 * BoneID);

			return matrix;
		}

		matrix3x4_t* GetBoneMatrix()
		{
			auto offset = *reinterpret_cast<uintptr_t*>(uintptr_t(this) + OFFSETS::m_dwBoneMatrix);
			if (!offset)
				return nullptr;

			return reinterpret_cast<matrix3x4_t*>(offset);
		}

		Vector GetPunchAngles()
		{
			return *reinterpret_cast<Vector*>(uintptr_t(this) + OFFSETS::m_aimPunchAngle);
		}

		bool GetImmunity()
		{
			return *reinterpret_cast<bool*>(uintptr_t(this) + OFFSETS::m_bGunGameImmunity);
		}

		bool SetupBones(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
		{
			void* pRenderable = reinterpret_cast<void*>(uintptr_t(this) + 0x4);
			if (!pRenderable)
				false;

			typedef bool (__thiscall* Fn)(void*, matrix3x4_t*, int, int, float);
			return VMT::GetVFunction<Fn>(pRenderable, 13)(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
		}

		void UpdateClientSideAnimation()
		{
			typedef void (__thiscall* original)(void*);
			VMT::GetVFunction<original>(this, 218)(this);
		}

		void SetAbsOrigin(Vector ArgOrigin)
		{
			using Fn = void(__thiscall*)(CBaseEntity*, const Vector& origin);
			static Fn func;

			if (!func)
				func = (Fn)(UTILS::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1\xE8\x00\x00", "xxxxxxxxxxxxx??"));

			func(this, ArgOrigin);
		}

		Vector& GetAbsAngles()
		{
			typedef Vector& (__thiscall* Fn)(void*);
			return (VMT::GetVFunction<Fn>(this, 11))(this);
		}

		void SetAbsAngles(Vector angles)
		{
			using Fn = void(__thiscall*)(CBaseEntity*, const Vector& angles);
			static Fn func;

			if (!func)
				func = (Fn)(UTILS::FindPattern("client.dll", (BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x64\x53\x56\x57\x8B\xF1\xE8", "xxxxxxxxxxxxxxx"));

			func(this, angles);
		}

		float* GetPoseParamaters()
		{
			return reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flPoseParameter);
		}

		Vector GetRenderAngles()
		{
			return *(Vector*)((DWORD)this + 0x128);
		}

		void SetRenderAngles(Vector angles)
		{
			*(Vector*)((DWORD)this + 0x128) = angles;
		}

		int DrawModel(int flags, uint8_t alpha)
		{
			void* pRenderable = (void*)(this + 0x4);

			using fn = int(__thiscall*)(void*, int, uint8_t);
			return VMT::GetVFunction<fn>(pRenderable, 9)(pRenderable, flags, alpha);
		}

		ClientClass* GetClientClass()
		{
			void* Networkable = (void*)(this + 0x8);
			typedef ClientClass*(__thiscall* OriginalFn)(void*);
			return VMT::GetVFunction<OriginalFn>(Networkable, 2)(Networkable);
		}

		float GetNextAttack()
		{
			return *reinterpret_cast<float*>(uint32_t(this) + OFFSETS::m_flNextAttack);
		}

		int GetActiveWeaponIndex()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_hActiveWeapon) & 0xFFF;
		}

		int GetArmor()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_ArmorValue);
		}

		bool HasHelmet()
		{
			return *reinterpret_cast<bool*>(uintptr_t(this) + OFFSETS::m_bHasHelmet);
		}

		bool IsScopedIn()
		{
			return *reinterpret_cast<bool*>(uintptr_t(this) + OFFSETS::m_bIsScoped);
		}

		uintptr_t* GetWeapons()
		{
			return reinterpret_cast<uintptr_t*>(uintptr_t(this) + OFFSETS::m_hMyWeapons);
		}

		int GetObserverMode()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_iObserverMode);
		}

		void SetObserverMode(int value)
		{
			*reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_iObserverMode) = value;
		}

		void SetFlashDuration(float value)
		{
			*reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flFlashDuration) = value;
		}

		void SetPunchAngles(Vector value)
		{
			*reinterpret_cast<Vector*>(uintptr_t(this) + OFFSETS::m_aimPunchAngle) = value;
		}

		void SetSpotted(bool value)
		{
			*reinterpret_cast<bool*>(uintptr_t(this) + OFFSETS::m_bSpotted) = value;
		}

		bool IsWeapon()
		{
			typedef bool (__thiscall* Fn)(void*);
			return VMT::GetVFunction<Fn>(this, 160)(this);
		}

		int GetMovetype()
		{
			return *reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_MoveType);
		}

		int GetViewModelIndex()
		{
			return (*reinterpret_cast<uintptr_t*>(uintptr_t(this) + OFFSETS::m_hViewModel)) & 0xFFF;
		}

		void SetModelIndex(int index)
		{
			*reinterpret_cast<int*>(uintptr_t(this) + OFFSETS::m_nModelIndex) = index;
		}

		void SetPoseAngles(float yaw, float pitch)
		{
			auto poses = GetPoseParamaters();
			poses[11] = (pitch + 90.f) / 180.f;
			poses[2] = (yaw + 180.f) / 360.f;
		}
	};
}
