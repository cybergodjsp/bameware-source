#include "../includes.h"

#include "Configurations.h"

namespace SETTINGS
{
	bool Configuration::Save(const char* file_name, const size_t size)
	{
		char file_path[MAX_PATH];
		sprintf(file_path, "C:\\Bameware\\Configs\\%s", file_name);

		/// Open the file, return false if failure
		std::fstream file(file_path, std::ios::trunc | std::ios::in | std::ios::out);
		if (!file.is_open())
			return false;

		/// Write each byte to file, 4 bytes per line, classes are always aligned by 4 bytes, 
		/// a class with an array of 2 chars will be 4 bytes big, not 2 which would be expected
		uintptr_t current_position = 0;
		for (; current_position < size; current_position += 4)
		{
			char message[16];
			sprintf(message, "0x%X ", *reinterpret_cast<unsigned long*>(this + current_position));
			file << message << std::endl;
		}

		/// Write the hash of the config at the end, to make sure nothing was tampered with
		char hash[64];
		sprintf(hash, "[H]0x%X", UTILS::FNVHash(this, size));
		file << hash << std::endl;

		file.close();
		return true;
	}

	bool Configuration::Load(const char* file_name, const size_t size)
	{
		char file_path[MAX_PATH];
		sprintf(file_path, "C:\\Bameware\\Configs\\%s", file_name);

		/// Open the file
		std::fstream file(file_path, std::ios::in | std::ios::out);
		if (!file.is_open())
			return false;

		/// Allocate memory to store the shit in until we can confirm that the hash is correct
		auto buffer = malloc(size);

		/// Fetch each line of the file until we reach the end
		std::string line;
		unsigned long counter = 0, hash = 0;
		while (std::getline(file, line))
		{
			/// It's the hash
			if (line[0] == '[' && line[1] == 'H' && line[2] == ']')
			{
				line.erase(line.begin(), line.begin() + 5);
				hash = strtoul(line.c_str(), nullptr, 16);
				break;
			}

			/// oof 
			*reinterpret_cast<unsigned long*>(uintptr_t(buffer) + counter) = strtoul(line.c_str(), nullptr, 16);

			counter += 4;
		}

		/// Dab on that file
		file.close();

		unsigned long new_hash = UTILS::FNVHash(buffer, size);

		/// File contents were changed externally
		if (hash != new_hash)
		{
			//free(buffer);
			//return false;
		}

		/// Copy over the contents from the buffer into the actual class
		memcpy(this, buffer, size);
		free(buffer);

		return true;
	}

	int Configuration::GetConfigs(char (*configs)[PPGUI::PPGUI_MAX_STRING_LENGTH], const char* extension)
	{
		int amount_of_configs = 0;

		WIN32_FIND_DATA ffd;
		auto hFind = FindFirstFile("C:\\Bameware\\Configs\\*", &ffd);
		while (FindNextFile(hFind, &ffd))
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			char file_name[260];
			strcpy(file_name, ffd.cFileName);

			/// file name length is less than the file extension length
			if (strlen(file_name) < strlen(extension))
				continue;

			/// file extension does not match
			if (strcmp(file_name + uintptr_t(strlen(file_name) - strlen(extension)), extension))
				continue;

			/// erase the file extension in the name
			file_name[strlen(file_name) - strlen(extension)] = '\0';

			strcpy(configs[amount_of_configs], file_name);
			amount_of_configs++;
		}

		return amount_of_configs;
	}

	bool Configuration::CreateConfig(const char* name, const char* extension)
	{
		char file_path[MAX_PATH];
		sprintf(file_path, "C:\\Bameware\\Configs\\%s%s", name, extension);

		/// Open the file, return false if failure
		std::fstream file(file_path, std::ios::trunc | std::ios::out);
		if (!file.is_open())
			return false;

		file.close();

		return true;
	}

	bool Configuration::DeleteConfig(const char* name, const char* extension)
	{
		char file_path[MAX_PATH];
		sprintf(file_path, "C:\\Bameware\\Configs\\%s%s", name, extension);

		return (remove(file_path) == 0);
	}

	bool Configuration::RenameConfig(const char* current_name, const char* new_name, const char* extension)
	{
		char current_file_path[MAX_PATH];
		sprintf(current_file_path, "C:\\Bameware\\Configs\\%s%s", current_name, extension);

		char new_file_path[MAX_PATH];
		sprintf(new_file_path, "C:\\Bameware\\Configs\\%s%s", new_name, extension);

		return (rename(current_file_path, new_file_path) == 0);
	}

	SkinConfig::SkinConfig()
	{
		for (int i = 0; i < FEATURES::MISC::skin_changer.NUM_WEAPONS; i++)
		{
			weapon_skin[i] = -1.f;
			weapon_quality[i] = -1.f;
			weapon_wear[i] = 0.f;
			weapon_seed[i] = 0.f;
			weapon_stat_trak_enabled[i] = false;
			weapon_stat_trak_kills[i] = 0;
			weapon_custom_name[i][0] = '\0';
		}
	}

	SkinConfig skin_configs;
	RagebotConfig ragebot_configs;
	MainConfig main_configs;
}
