#pragma once

#include <string>

namespace carpet {
int getPortFromEnv(const std::string& port_env_name);

std::string getAddressFromEnv(const std::string& host, const std::string& port_env_name);

std::string getComponentIp(const std::string &name);
} // namespace carpet
