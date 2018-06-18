#pragma once

#define LOG(x) UTILS::Log(x)

namespace UTILS
{
	void Log(float message);

	void Log(std::string message);
}
