#include "../includes.h"

#include "Debugger.h"

namespace UTILS
{
	namespace DEBUGGER
	{
		Debugger debugger;

		void Debugger::Initialize()
		{
			createmove_profiler.Initialize("Createmove", 5);
		}
	}
}
