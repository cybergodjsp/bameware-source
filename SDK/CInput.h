#pragma once
#include "../includes.h"
#include "CUserCmd.h"

class CInput
{
public:
	SDK::CUserCmd* GetUserCmd(int sqnum)
	{
		typedef SDK::CUserCmd*(__thiscall* Fn)(void*, int, int);
		return (VMT::GetVFunction<Fn>(this, 8))(this, 0, sqnum);
	}
};
