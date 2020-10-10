#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

namespace cms {
  void split(std::vector<std::string>& tokens, const std::string& s, char delimiter);

  template<typename InputIt>
  std::string _join(
    InputIt begin,
    InputIt end,
    const std::string & separator =",",  // see 1.
    const std::string & concluder ="")    // see 1.
  {
    std::ostringstream ss;

    if(begin != end)
    {
      ss << *begin++; // see 3.
    }    

    while(begin != end) // see 3.
    {
      ss << separator;
      ss << *begin++;
    }

    ss << concluder;
    return ss.str();
  }

  
  inline std::string join(
    const std::vector<std::string>& v,

    const std::string& separator =",",
    const std::string& concluder ="") {
      return _join(v.begin(), v.end(), separator, concluder);
  }
}
