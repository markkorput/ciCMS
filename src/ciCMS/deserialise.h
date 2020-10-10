#pragma once 

#include <string>
#include "glm/glm.hpp"
#include "cinder/Color.h"

namespace cms {

  namespace serde {
    void vec2(glm::vec2& target, const std::string& str, const glm::vec2& defaultValue);
    void vec3(glm::vec3& target, const std::string& str, const glm::vec3& defaultValue);
    void vec4(glm::vec4& target, const std::string& str, const glm::vec4& defaultValue);
  }

  template<typename T>
  T deserialise(const std::string& str);

  int deserialiseInt(const std::string& str, int defaultValue);
  float deserialiseFloat(const std::string& str, float defaultValue);
  bool deserialiseBool(const std::string& str, bool defaultValue);
  inline void deserialiseVec2(glm::vec2& target, const std::string& str, const glm::vec2& defaultValue) { 
    return serde::vec2(target, str, defaultValue);
  }
  glm::vec3 deserialiseVec3(const std::string& str, const glm::vec3& defaultValue);
  glm::vec4 deserialiseVec4(const std::string& str, const glm::vec4& defaultValue);
  cinder::ColorAf deserialiseColor(const std::string& str, const cinder::ColorAf& defaultValue);

  glm::ivec2 deserialise_ivec2(const std::string& str, const glm::ivec2& defaultValue);


}
