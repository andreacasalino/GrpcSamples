#include <Config.h>
#include <Error.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

namespace carpet {
int getPortFromEnv(const std::string& port_env_name) {
  const auto* str = std::getenv(port_env_name.c_str());
  if(!str) {
    THROW_ERROR(port_env_name,"was not found in the environment");
  }
  return std::atoi(str);
}

std::string getAddressFromEnv(const std::string& host, const std::string& port_env_name) {
  return host + std::to_string(getPortFromEnv(port_env_name));
}

std::string getComponentIp(const std::string &name) {
  if (std::getenv("IS_FROM_DOCKER")) {
    struct hostent *he;
    he = gethostbyname(name.c_str());
    if (he == NULL) { // do some error checking
      THROW_ERROR("Unable to find ip for: ", name);
    }
    std::stringstream tmp;
    tmp << inet_ntoa(*(struct in_addr *)he->h_addr);
    return tmp.str();
  }
  return "0.0.0.0";
}
} // namespace carpet
