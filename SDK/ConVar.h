#pragma once

namespace SDK
{
	class ConVar
	{
	public:
		char pad_0x0000[0x4];
		ConVar* pNext;
		int32_t bRegistered;
		char* pszName;
		char* pszHelpString;
		int32_t nFlags;
		char pad_0x0018[0x4];
		ConVar* pParent;
		char* pszDefaultValue;
		char* strString;
		int32_t StringLength;
		float fValue;
		int32_t nValue;
		int32_t bHasMin;
		float fMinVal;
		int32_t bHasMax;
		float fMaxVal;
		void* fnChangeCallback;

		float GetFloat(void) const
		{
			auto temp = *(int*)(&fValue);
			auto temp_result = (int)(temp ^ (DWORD)this);

			return *(float*)(&temp_result);
		}

		int GetInt(void) const
		{
			auto temp = *(int*)(&fValue);
			auto temp_result = (int)(temp ^ (DWORD)this);

			return *(int*)(&temp_result);
		}

		bool GetBool(void) const
		{
			return !!GetInt();
		}

		void SetValue(const char* value)
		{
			typedef void (__thiscall* OriginalFn)(void*, const char*);
			return VMT::GetVFunction<OriginalFn>(this, 14)(this, value);
		}
	};//Size=0x0048

	class ICVar
	{
	public:
		ConVar* FindVar(const char* var_name)
		{
			typedef ConVar*(__thiscall* FindVarFn)(void*, const char*);
			return VMT::GetVFunction<FindVarFn>(this, 14)(this, var_name);
		}

		void ConsoleColorPrintf(const CColor& col, const char* pFormat)
		{
			typedef void*(__thiscall* Fn)(void*, const CColor&, const char*);
			VMT::GetVFunction<Fn>(this, 24)(this, col, pFormat);
		}
	};
}
