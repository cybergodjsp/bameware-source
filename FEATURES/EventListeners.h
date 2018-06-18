#pragma once

#include "../SDK/GameEvents.h"

#define CREATE_EVENT_LISTENER(class_name, event_name)\
class class_name : public SDK::IGameEventListener\
{\
public:\
	~class_name() { INTERFACES::GameEventManager->RemoveListener(this); }\
	virtual void FireGameEvent(SDK::IGameEvent* game_event);\
};

namespace FEATURES
{
	namespace MISC
	{
		void InitializeEventListeners();
		void RemoveEventListeners();
	}
}
