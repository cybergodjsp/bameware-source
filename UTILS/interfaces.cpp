#include "..\includes.h"

#include "interfaces.h"

namespace INTERFACES
{
	SDK::IClient* Client;
	SDK::CInput* Input;
	SDK::CPanel* Panel;
	SDK::ISurface* Surface;
	SDK::IEngine* Engine;
	SDK::CClientEntityList* ClientEntityList;
	SDK::CGlobalVars* Globals;
	SDK::IVDebugOverlay* DebugOverlay;
	SDK::CTrace* Trace;
	SDK::CModelInfo* ModelInfo;
	SDK::ICVar* cvar;
	SDK::CInputSystem* InputSystem;
	SDK::CModelRender* ModelRender;
	SDK::CRenderView* RenderView;
	SDK::IMaterialSystem* MaterialSystem;
	SDK::IPhysicsProps* PhysicsProps;
	SDK::CClientMode* ClientMode;
	SDK::IGameEventManager* GameEventManager;

	SDK::IGameMovement* GameMovement;
	SDK::CPrediction* Prediction;
	SDK::IMoveHelper* MoveHelper;

	void InitInterfaces()
	{
		Client = reinterpret_cast<SDK::IClient*>(FindInterface(enc_char("client.dll"), enc_char("VClient")));
		Input = *reinterpret_cast<SDK::CInput**>((*(DWORD**)Client)[15] + 0x1);
		Panel = reinterpret_cast<SDK::CPanel*>(FindInterface(enc_char("vgui2.dll"), enc_char("VGUI_Panel")));
		Surface = reinterpret_cast<SDK::ISurface*>(FindInterface(enc_char("vguimatsurface"), enc_char("VGUI_Surface")));
		Engine = reinterpret_cast<SDK::IEngine*>(FindInterface(enc_char("engine.dll"), enc_char("VEngineClient")));
		ClientEntityList = reinterpret_cast<SDK::CClientEntityList*>(FindInterface(enc_char("client.dll"), enc_char("VClientEntityList")));
		DebugOverlay = reinterpret_cast<SDK::IVDebugOverlay*>(FindInterface(enc_char("engine.dll"), enc_char("VDebugOverlay")));
		Trace = reinterpret_cast<SDK::CTrace*>(FindInterface(enc_char("engine.dll"), enc_char("EngineTraceClient")));
		ModelInfo = reinterpret_cast<SDK::CModelInfo*>(FindInterface(enc_char("engine.dll"), enc_char("VModelInfoClient")));
		cvar = reinterpret_cast<SDK::ICVar*>(FindInterface(enc_char("vstdlib.dll"), enc_char("VEngineCvar")));
		InputSystem = reinterpret_cast<SDK::CInputSystem*>(FindInterface(enc_char("inputsystem.dll"), enc_char("InputSystemVersion")));
		ModelRender = reinterpret_cast<SDK::CModelRender*>(FindInterface(enc_char("engine.dll"), enc_char("VEngineModel")));
		RenderView = reinterpret_cast<SDK::CRenderView*>(FindInterface(enc_char("engine.dll"), enc_char("VEngineRenderView")));
		PhysicsProps = reinterpret_cast<SDK::IPhysicsProps*>(FindInterface(enc_char("vphysics.dll"), enc_char("VPhysicsSurfaceProps")));
		GameEventManager = reinterpret_cast<SDK::IGameEventManager*>(FindInterfaceEx(enc_char("engine.dll"), enc_char("GAMEEVENTSMANAGER002")));

		GameMovement = reinterpret_cast<SDK::IGameMovement*>(FindInterface(enc_char("client.dll"), enc_char("GameMovement")));
		Prediction = reinterpret_cast<SDK::CPrediction*>(FindInterface(enc_char("client.dll"), enc_char("VClientPrediction")));
		MoveHelper = **reinterpret_cast<SDK::IMoveHelper***>((UTILS::FindPattern(/*enc_char(*/"client.dll"/*)*/, (PBYTE)/*enc_char(*/"\x8B\x0D\x00\x00\x00\x00\x8B\x46\x08\x68"/*)*/, /*enc_char(*/"xx????xxxx"/*)*/) + 2));

		Globals = **reinterpret_cast<SDK::CGlobalVars***>((*reinterpret_cast<DWORD**>(Client))[0] + 0x1B);

		std::cout << std::hex << Globals << std::endl;

		void** pClientTable = *reinterpret_cast<void***>(Client);
		ClientMode = **reinterpret_cast<SDK::CClientMode***>(reinterpret_cast<DWORD>(pClientTable[10]) + 5);
	}

	void* FindInterface(const char* Module, const char* InterfaceName)
	{
		void* Interface = nullptr;
		auto CreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(
			GetModuleHandleA(Module), enc_char("CreateInterface")));

		char PossibleInterfaceName[1024];
		for (int i = 1; i < 100; i++)
		{
			sprintf(PossibleInterfaceName, "%s0%i", InterfaceName, i);
			Interface = CreateInterface(PossibleInterfaceName, 0);
			if (Interface)
				break;

			sprintf(PossibleInterfaceName, "%s00%i", InterfaceName, i);
			Interface = CreateInterface(PossibleInterfaceName, 0);
			if (Interface)
				break;
		}

		if (!Interface)
		LOG(enc_str("Failed to find interface - ") + PossibleInterfaceName);
		else
		LOG(enc_str("Successfully found interface - ") + PossibleInterfaceName);

		return Interface;
	}

	void* FindInterfaceEx(const char* Module, const char* InterfaceName)
	{
		void* Interface = nullptr;
		auto CreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(
			GetModuleHandleA(Module), "CreateInterface"));

		Interface = CreateInterface(InterfaceName, 0);
		if (!Interface)
			std::cout << InterfaceName << " not found!" << std::endl;

		return Interface;
	}
}
