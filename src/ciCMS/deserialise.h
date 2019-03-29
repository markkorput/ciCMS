#include <string>
#include "glm/glm.hpp"
#include "cinder/Color.h"

namespace cms {
  template<typename T>
  T deserialise(const std::string& str);

  int deserialiseInt(const std::string& str, int defaultValue);
  float deserialiseFloat(const std::string& str, float defaultValue);
  bool deserialiseBool(const std::string& str, bool defaultValue);
  glm::vec2 deserialiseVec2(const std::string& str, const glm::vec2& defaultValue);
  glm::vec3 deserialiseVec3(const std::string& str, const glm::vec3& defaultValue);
  glm::vec4 deserialiseVec4(const std::string& str, const glm::vec4& defaultValue);
  cinder::ColorAf deserialiseColor(const std::string& str, const cinder::ColorAf& defaultValue);
}
