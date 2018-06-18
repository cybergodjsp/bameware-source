#pragma once

#include "../SDK/CMoveData.h"
#include "../SDK/CMoveHelper.h"
#include "../SDK/CPrediction.h"
#include "../SDK/GameMovement.h"

namespace FEATURES
{
	namespace MISC
	{
		class EnginePrediction
		{
		public:
			float m_old_curtime;
			float m_old_frametime;

			void StartPrediction(SDK::CUserCmd* cmd);
			void EndPrediction() const;

		private:
			SDK::CMoveData m_MoveData;

			int* m_pPredictionRandomSeed;
		};

		extern EnginePrediction engine_prediction;
	}
}
