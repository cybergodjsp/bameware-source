#pragma once
#include "INetChannelInfo.h"
#include "player_info_t.h"
#include "../UTILS/offsets.h"

namespace SDK
{
	class NetChannel;

	class IEngine
	{
	public:
		int GetPlayerForUserID(int id)
		{
			typedef int (__thiscall* Fn)(void*, int);
			return (VMT::GetVFunction<Fn>(this, 9))(this, id);
		}

		INetChannelInfo* GetNetChannelInfo()
		{
			typedef INetChannelInfo*(__thiscall* Fn)(void*);
			return (VMT::GetVFunction<Fn>(this, 78))(this);
		}

		bool GetPlayerInfo(int Index, player_info_t* PlayerInfo)
		{
			typedef bool (__thiscall* Fn)(void*, int, player_info_t*);
			return (VMT::GetVFunction<Fn>(this, 8))(this, Index, PlayerInfo);
		}

		void GetScreenSize(int& Width, int& Height)
		{
			typedef void (__thiscall* Fn)(void*, int&, int&);
			return (VMT::GetVFunction<Fn>(this, 5))(this, Width, Height);
		}

		int GetLocalPlayer()
		{
			typedef int (__thiscall* Fn)(void*);
			return (VMT::GetVFunction<Fn>(this, 12))(this);
		}

		void ClientCmd(const char* Command)
		{
			typedef void (__thiscall* Fn)(void*, const char*);
			return (VMT::GetVFunction<Fn>(this, 108))(this, Command);
		}

		void SetViewAngles(Vector& Angles)
		{
			typedef void (__thiscall* Fn)(void*, Vector&);
			return (VMT::GetVFunction<Fn>(this, 19))(this, Angles);
		}

		void GetViewAngles(Vector& angle)
		{
			typedef void (__thiscall* Fn)(void*, Vector&);
			return (VMT::GetVFunction<Fn>(this, 18))(this, angle);
		}

		const matrix3x4_t& GetMatrix()
		{
			typedef const matrix3x4_t& (__thiscall* Fn)(void*);
			return (VMT::GetVFunction<Fn>(this, 37))(this);
		}

		bool IsInGame()
		{
			typedef bool (__thiscall* Fn)(void*);
			return (VMT::GetVFunction<Fn>(this, 26))(this);
		}

		bool IsConnected()
		{
			typedef bool (__thiscall* Fn)(void*);
			return (VMT::GetVFunction<Fn>(this, 27))(this);
		}

		NetChannel* GetNetChannel()
		{
			typedef NetChannel*(__thiscall* Fn)(void*);
			return VMT::GetVFunction<Fn>(this, 78)(this);
		}

		static uintptr_t GetClientState()
		{
			static uintptr_t ClientStateAddr = UTILS::FindSignature(enc_char("engine.dll"), enc_char("A1 ? ? ? ? 33 D2 6A 00 6A 00 33 C9 89 B0"));

			if (!ClientStateAddr)
				return 0;

			return **reinterpret_cast<uintptr_t**>(ClientStateAddr + 0x1);
		}

		static void ForceFullUpdate()
		{
			static auto client_state = GetClientState();
			if (!client_state)
				return;

			*reinterpret_cast<int*>(client_state + 0x174) = -1;
		}
	};
}
