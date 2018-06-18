#pragma once

#include "../UTILS/vmt.h"

namespace SDK
{
	class CPanel
	{
	public:
		const char* GetName(unsigned int Panel)
		{
			typedef const char*(__thiscall* Fn)(void*, unsigned int);
			return (VMT::GetVFunction<Fn>(this, 36))(this, Panel);
		}

		void SetMouseInputEnabled(unsigned int iPanel, bool bState)
		{
			return (VMT::GetVFunction<void(__thiscall *)(PVOID, int, bool)>(this, 32))(this, iPanel, bState);
		}
	};
}
