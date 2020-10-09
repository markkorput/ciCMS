#include "utils.h"

#include <string>
#include <sstream>

namespace cms { namespace cfg {
  void split(std::vector<std::string>& tokens, const std::string& s, char delimiter)
  {
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
  }
}}