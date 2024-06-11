#pragma once

#include <string>

namespace carpet {
int getPortFromEnv(const std::string& server_name);

std::string getComponentIp(const std::string &name);
} // namespace carpet
