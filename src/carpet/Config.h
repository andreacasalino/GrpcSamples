#pragma once

#include <string>

namespace trd {
std::uint16_t getPortFromEnv(const std::string& server_name);

std::string getComponentIp(const std::string &name);
} // namespace trd
