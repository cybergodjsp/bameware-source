#pragma once

namespace SDK
{
	class CRecvProxyData;
	struct RecvTable;
}

namespace UTILS
{
	typedef void (*NetvarHookCallback)(const SDK::CRecvProxyData* pData, void* pStruct, void* pOut);

	class CNetvarManager
	{
	public:
		void* Hook(std::string table, std::string prop, NetvarHookCallback callback);

		uintptr_t GetOffset(std::string table, std::string prop);

		void DumpNetvars();

	private:
		uintptr_t RecursivelyFindOffset(SDK::RecvTable* table, std::string table_name, std::string prop_name);
		bool RecursivelyDumpTable(SDK::RecvTable* table, int depth, uint32_t previous_hash);
	};

	extern CNetvarManager netvar_manager;
}
