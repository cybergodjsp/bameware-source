#pragma once

namespace SDK
{
	class IMatRenderContext;
	class ITraceFilter;
	class NetChannel;

	struct CViewSetup;
	struct Ray_t;
	struct DrawModelState_t;
	struct ModelRenderInfo_t;
	struct trace_t;
}

namespace HOOKS
{
	typedef bool (__thiscall* CreateMoveFn)(void*, float, SDK::CUserCmd*);
	typedef void (__thiscall* FrameStageNotifyFn)(void*, int);
	typedef void (__thiscall* PaintTraverseFn)(void*, unsigned int, bool, bool);
	typedef void (__thiscall* DrawModelExecuteFn)(void*, SDK::IMatRenderContext*, const SDK::DrawModelState_t&, const SDK::ModelRenderInfo_t&, matrix3x4_t*);
	typedef void (__thiscall* SceneEndFn)(void*);
	typedef void (__thiscall* TraceRayFn)(void* thisptr, const SDK::Ray_t& ray, unsigned int fMask, SDK::ITraceFilter* pTraceFilter, SDK::trace_t* pTrace);
	typedef int (__thiscall* SendDatagramFn)(void* thisptr, void* data);
	typedef void (__thiscall* OverrideViewFn)(void*, SDK::CViewSetup*);
	typedef bool (__thiscall* SetupBonesFn)(void*, matrix3x4_t*, int, int, float);

	extern CreateMoveFn original_create_move;
	extern PaintTraverseFn original_paint_traverse;
	extern FrameStageNotifyFn original_frame_stage_notify;
	extern DrawModelExecuteFn original_draw_model_execute;
	extern SceneEndFn original_scene_end;
	extern TraceRayFn original_trace_ray;
	extern SendDatagramFn original_send_datagram;
	extern OverrideViewFn original_override_view;
	extern SetupBonesFn original_setup_bones;

	extern VMT::VMTHookManager iclient_hook_manager;
	extern VMT::VMTHookManager panel_hook_manager;
	extern VMT::VMTHookManager model_render_hook_manager;
	extern VMT::VMTHookManager render_view_hook_manager;
	extern VMT::VMTHookManager trace_hook_manager;
	extern VMT::VMTHookManager net_channel_hook_manager;
	extern VMT::VMTHookManager client_mode_hook_manager;
	extern VMT::VMTHookManager setup_bones_hook_manager;

	void InitHooks();
	void UnHook();

	/// netvar hooks
	extern void* OriginalSequenceFn;

	void InitNetvarHooks();
}
