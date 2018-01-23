#include "deserialise.h"

#ifndef CINDER_MSW
namespace cms {

  template<>
  std::string fromstr<std::string>(const std::string& str){
    return str;
  }

  template<>
  int fromstr<int>(const std::string& str){
    return std::stoi(str);
  }
}
#endif
