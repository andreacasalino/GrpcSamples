#pragma once

#include <string>

namespace carpet {
std::uint16_t getPortFromEnv(const std::string& server_name);

std::string getComponentIp(const std::string &name);
} // namespace carpet
