#pragma once

namespace SDK
{
	class CMoveData;

	class CPrediction
	{
	public:
		virtual void UnknownVirtual0() = 0;
		virtual void UnknownVirtual1() = 0;
		virtual void UnknownVirtual2() = 0;
		virtual void UnknownVirtual3() = 0;
		virtual void UnknownVirtual4() = 0;
		virtual void UnknownVirtual5() = 0;
		virtual void UnknownVirtual6() = 0;
		virtual void UnknownVirtual7() = 0;
		virtual void UnknownVirtual8() = 0;
		virtual void UnknownVirtual9() = 0;
		virtual void UnknownVirtual10() = 0;
		virtual void UnknownVirtual11() = 0;
		virtual void UnknownVirtual12() = 0;
		virtual void UnknownVirtual13() = 0;
		virtual void UnknownVirtual14() = 0;
		virtual void UnknownVirtual15() = 0;
		virtual void GetLocalViewAngles(Vector& ang);
		virtual void SetLocalViewAngles(Vector& ang);

		virtual void CheckMovingGround(CBaseEntity* player, double frametime);
		virtual void RunCommand(CBaseEntity* player, CUserCmd* ucmd, IMoveHelper* moveHelper);

		virtual void SetupMove(CBaseEntity* player, CUserCmd* ucmd, IMoveHelper* pHelper, CMoveData* move) = 0; //20
		virtual void FinishMove(CBaseEntity* player, CUserCmd* ucmd, CMoveData* move) = 0;
	};
}
