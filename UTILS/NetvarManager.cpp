#include "../includes.h"

#include "interfaces.h"
#include "../SDK/IClient.h"
#include "../SDK/RecvData.h"

#include "NetvarManager.h"

namespace UTILS
{
	CNetvarManager netvar_manager;

	void* CNetvarManager::Hook(std::string table_to_hook, std::string prop_to_hook, NetvarHookCallback callback)
	{
		auto client_class = INTERFACES::Client->GetAllClasses();

		// go through all classes
		while (client_class)
		{
			std::string table_name = client_class->m_pRecvTable->m_pNetTableName;

			if (table_name == table_to_hook)
			{
				for (int i = 0; i < client_class->m_pRecvTable->m_nProps; i++)
				{
					auto& prop = client_class->m_pRecvTable->m_pProps[i];
					std::string prop_name = prop.m_pVarName;

					if (prop_name == prop_to_hook)
					{
						LOG("Successfully hooked netvar proxy - " + prop_to_hook);
						auto original = prop.m_ProxyFn;
						prop.m_ProxyFn = callback;
						return original;
					}
				}
			}

			client_class = client_class->m_pNext; // go to the next class
		}

		return nullptr;
	}

	uintptr_t CNetvarManager::GetOffset(std::string table, std::string prop)
	{
		auto client_class = INTERFACES::Client->GetAllClasses();

		// go through all classes
		while (client_class)
		{
			auto return_value = RecursivelyFindOffset(client_class->m_pRecvTable, table, prop);
			if (return_value)
				return return_value;

			client_class = client_class->m_pNext; // go to the next class
		}

		LOG("** Failed to get offset - " + prop);
		return 0;
	}

	uintptr_t CNetvarManager::RecursivelyFindOffset(SDK::RecvTable* table, std::string table_name, std::string prop_name)
	{
		for (int i = 0; i < table->m_nProps; i++)
		{
			if (table_name == table->m_pNetTableName && prop_name == table->m_pProps[i].m_pVarName)
			{
				LOG(std::string("Successfully found offset ") + table->m_pNetTableName + " " + table->m_pProps[i].m_pVarName);
				return table->m_pProps[i].m_Offset;
			}

			if (!table->m_pProps[i].m_pDataTable)
				continue;

			auto return_value = RecursivelyFindOffset(table->m_pProps[i].m_pDataTable, table_name, prop_name);
			if (return_value)
				return return_value;
		}

		return 0;
	}

	void CNetvarManager::DumpNetvars()
	{
		auto client_class = INTERFACES::Client->GetAllClasses();

		// go through all classes
		while (client_class)
		{
			if (!RecursivelyDumpTable(client_class->m_pRecvTable, 0, 0))
				return;

			client_class = client_class->m_pNext; // go to the next class
		}
	}

	bool CNetvarManager::RecursivelyDumpTable(SDK::RecvTable* table, int depth, uint32_t previous_hash)
	{
		static std::vector<uint32_t> hashes;
		for (int i = 0; i < table->m_nProps; i++)
		{
			std::stringstream sstream;
			sstream << std::hex << table->m_pProps[i].m_Offset;
			std::string offset = "0x" + sstream.str();
			std::string message = std::string(depth * 2, ' ') + std::string(table->m_pNetTableName) + " " + table->m_pProps[i].m_pVarName;
			message = message + " " + std::string(125 - message.size(), '-') + " " + offset;

			/// so that we dont dump the same netvar twice
			uint32_t current_hash = FNVHash(message.data(), message.size() * 4);
			bool is_duplicate = false;
			for (const uint32_t& hash : hashes)
			{
				if (hash == current_hash)
				{
					is_duplicate = true;
					break;
				}
			}
			if (!is_duplicate)
			{
				hashes.push_back(current_hash);
				LOG(message);
			}

			if (table->m_pProps[i].m_pDataTable)
				RecursivelyDumpTable(table->m_pProps[i].m_pDataTable, depth + 1, current_hash);
		}
		return true;
	}
}
