#pragma once

namespace SDK
{
	class IVDebugOverlay
	{
	public:
		int ScreenPosition(Vector& point, Vector& screen)
		{
			typedef int (__thiscall *Fn)(void*, Vector&, Vector&);
			return VMT::GetVFunction<Fn>(this, 13)(this, point, screen);
		}
	};
}
