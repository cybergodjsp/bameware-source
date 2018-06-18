#include "../includes.h"

namespace UTILS
{
	void Log(float message)
	{
		Log(std::to_string(message));
	}

	void Log(std::string message)
	{
		std::fstream file;

		static char file_path[MAX_PATH];
		if (static bool is_init = false;
		!is_init
		)
		{
			is_init = true;

			const auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::strftime(file_path, 256, "C:\\Bameware\\Logs\\%d-%m-%Y-%H.log", std::localtime(&time));

			is_init = true;
			file.open(file_path, std::ios::out | std::ios::in | std::ios::trunc);
		}
		else
		file.open(file_path, std::ios::out | std::ios::in | std::ios::app);

		if (!file.is_open())
			return;

		file << message << std::endl;
		file.close();
	}
}
