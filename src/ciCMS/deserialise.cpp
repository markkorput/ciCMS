#include <vector>
#include "deserialise.h"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"


#ifndef CINDER_MSW
namespace cms {

  template<>
  std::string deserialise<std::string>(const std::string& str){ return str; }

  template<>
  int deserialise<int>(const std::string& str){ return cms::deserialiseInt(str, 0); }

  template<>
  float deserialise<float>(const std::string& str){ return cms::deserialiseFloat(str, 0.0f); }

  template<>
  bool deserialise<bool>(const std::string& str){ return cms::deserialiseBool(str, false); }

  template<>
  glm::vec2 deserialise<glm::vec2>(const std::string& str){ return cms::deserialiseVec2(str, glm::vec2(0.0f, 0.0f)); }

  template<>
  glm::vec3 deserialise<glm::vec3>(const std::string& str){ return cms::deserialiseVec3(str, glm::vec3(0.0f, 0.0f, 0.0f)); }

  template<>
  cinder::ColorAf deserialise<cinder::ColorAf>(const std::string& str){ return cms::deserialiseColor(str, ci::ColorAf(1.0f, 1.0f, 1.0f, 1.0f)); }


  int deserialiseInt(const std::string& str, int defaultValue){
    try {
      return std::stoi(str);
    } catch(std::invalid_argument exc){
      // std::cerr << exc.what();
    }

    return defaultValue;
  }

  float deserialiseFloat(const std::string& str, float defaultValue){
    try {
      return std::stof(str);
    } catch(std::invalid_argument exc){
      // std::cerr << exc.what();
    }

    return defaultValue;
  }

  bool deserialiseBool(const std::string& str, bool defaultValue){
    std::string s = str;

    if(s == "true")
      return true;

    try {
      return boost::lexical_cast<bool>(s);
    } catch(boost::bad_lexical_cast exc){
      // std::cerr << exc.what();
    }

    return defaultValue;
  }


  glm::vec2 deserialiseVec2(const std::string& str, const glm::vec2& defaultValue){
    std::vector<std::string> strings;

    boost::split(strings, str, boost::is_any_of(","));

    try {
      if(strings.size() == 2)
        return glm::vec2(std::stof(strings[0]), std::stof(strings[1]));

      if(strings.size() == 1 && strings[0] != ""){
        float val = std::stof(strings[0]);
        return glm::vec2(val, val);
      }
    } catch(std::invalid_argument exc){
      // std::cerr << exc.what();
    }

    return defaultValue;
  }

  glm::vec3 deserialiseVec3(const std::string& str, const glm::vec3& defaultValue){
      std::vector<std::string> strings;
      boost::split(strings, str, boost::is_any_of(","));

      try{
          if(strings.size() == 3)
              return glm::vec3(std::stof(strings[0]), std::stof(strings[1]), std::stof(strings[2]));

          if(strings.size() == 1 && strings[0] != ""){
              float val = std::stof(strings[0]);
              return glm::vec3(val, val, val);
          }
      } catch(std::invalid_argument exc){
          // std::cerr << exc.what();
      }

      return defaultValue;
  }

  glm::vec4 deserialiseVec4(const std::string& str, const glm::vec4& defaultValue){
      std::vector<std::string> strings;
      boost::split(strings, str, boost::is_any_of(","));

      try{
          if(strings.size() == 4)
              return glm::vec4(std::stof(strings[0]), std::stof(strings[1]), std::stof(strings[2]), std::stof(strings[3]));

          if(strings.size() == 1 && strings[0] != ""){
              float val = std::stof(strings[0]);
              return glm::vec4(val, val, val, val);
          }
      } catch(std::invalid_argument exc){
          // std::cerr << exc.what();
      }

      return defaultValue;
  }


  cinder::ColorAf deserialiseColor(const std::string& str, const ci::ColorAf& defaultValue){
    std::vector<std::string> strings;
    boost::split(strings, str, boost::is_any_of(","));

    try{
        if(strings.size() == 3)
            return ci::ColorAf(std::stof(strings[0])/255.0f, std::stof(strings[1])/255.0f, std::stof(strings[2])/255.0f, 1.0f);

        if(strings.size() == 4)
            return ci::ColorAf(std::stof(strings[0])/255.0f, std::stof(strings[1])/255.0f, std::stof(strings[2])/255.0f, std::stof(strings[3])/255.0f);

        if(strings.size() == 1 && strings[0] != ""){
            float val = std::stof(strings[0]);
            return ci::ColorAf(val/255.0f, val/255.0f, val/255.0f, 1.0f);
        }
    } catch(std::invalid_argument exc){
        // CI_LOG_E("-- ciCMS err --\n" << exc.what() << "\n\n-- ciCMS err end --");
    }
    // unsupported/unrecognised format
    return defaultValue;
  }
}
#endif
