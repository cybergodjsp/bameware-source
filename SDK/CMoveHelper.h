#pragma once

namespace SDK
{
	class IMoveHelper
	{
	private:
		virtual void UnknownVirtual() = 0;
	public:
		virtual void SetHost(CBaseEntity* host) = 0;
	};
}
