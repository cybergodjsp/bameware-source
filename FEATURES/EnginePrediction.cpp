#include "../includes.h"

#include "EnginePrediction.h"

#include "../UTILS/interfaces.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CUserCmd.h"

namespace FEATURES
{
	namespace MISC
	{
		EnginePrediction engine_prediction;

		void EnginePrediction::StartPrediction(SDK::CUserCmd* cmd)
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			static bool is_initialized = false;
			if (!is_initialized)
			{
				m_pPredictionRandomSeed = *(int**)(UTILS::FindPattern("client.dll", (PBYTE)"\x8B\x0D\00\x00\x00\x00\xBA\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x04", "xx????x????x????xxx") + 2);
				is_initialized = true;
			}

			*m_pPredictionRandomSeed = cmd->random_seed;

			m_old_curtime = INTERFACES::Globals->curtime;
			m_old_frametime = INTERFACES::Globals->frametime;
			INTERFACES::Globals->curtime = UTILS::GetCurtime();
			INTERFACES::Globals->frametime = INTERFACES::Globals->interval_per_tick;

			INTERFACES::GameMovement->StartTrackPredictionErrors(local_player);

			memset(&m_MoveData, 0, sizeof(m_MoveData));
			INTERFACES::MoveHelper->SetHost(local_player);
			INTERFACES::Prediction->SetupMove(local_player, cmd, INTERFACES::MoveHelper, &m_MoveData);
			INTERFACES::GameMovement->ProcessMovement(local_player, &m_MoveData);
			INTERFACES::Prediction->FinishMove(local_player, cmd, &m_MoveData);
		}

		void EnginePrediction::EndPrediction() const
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			INTERFACES::GameMovement->FinishTrackPredictionErrors(local_player);
			INTERFACES::MoveHelper->SetHost(0);

			*m_pPredictionRandomSeed = -1;

			INTERFACES::Globals->curtime = m_old_curtime;
			INTERFACES::Globals->frametime = m_old_frametime;
		}
	}
}
