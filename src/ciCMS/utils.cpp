#include "utils.h"

#include <iostream>
#include <string>
#include <sstream>

namespace cms {
  void split(std::vector<std::string>& tokens, const std::string& s, char delimiter)
  {
    if (s.length() == 0) return;

    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
      tokens.push_back(token);
    }
  }
}