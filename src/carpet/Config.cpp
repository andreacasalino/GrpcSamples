#include <Config.h>
#include <Error.h>

#include <arpa/inet.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <netdb.h>
#include <netinet/in.h>
#include <optional>
#include <sstream>
#include <stdio.h>
#include <string_view>
#include <sys/socket.h>

namespace trd {
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
} // namespace trd
