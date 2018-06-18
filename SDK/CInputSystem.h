#pragma once

namespace SDK
{
	class CInputSystem
	{
	public:
		void EnableInput(bool enabled)
		{
			typedef void (__thiscall* Fn)(void*, bool);
			return (VMT::GetVFunction<Fn>(this, 11))(this, enabled);
		}
	};
}
