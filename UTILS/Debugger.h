#pragma once

#include "TimeProfiler.h"

namespace UTILS
{
	namespace DEBUGGER
	{
		class Debugger
		{
		public:
			TimeProfiler createmove_profiler;

			void Initialize();

		private:
		};

		extern Debugger debugger;
	}
}
