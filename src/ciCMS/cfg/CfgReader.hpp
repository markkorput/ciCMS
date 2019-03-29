#include <map>
#include <regex>
#include "ciCMS/deserialise.h"

namespace cms { namespace cfg {
  class CfgReader : public std::map<std::string, std::string> {
  public:
    static const CfgReader& read(const std::map<std::string, std::string>& data) {
      return *((CfgReader*)&data);
    }

    bool has(const string& attr) const {
      return this->find(attr) != this->end();
    }

  public: // "with" methods

    const CfgReader& with(const string& attr, function<void(const string&)> func) const {
      if(this->has(attr))
        func(this->at(attr));
      return *this;
    }

    const CfgReader& withInt(const string& attr, function<void(const int&)> func) const {
      if(this->has(attr))
        func(cms::deserialiseInt(this->at(attr), 0));
      return *this;
    }

    const CfgReader& withFloat(const string& attr, function<void(float)> func) const {
      if(this->has(attr))
        func(cms::deserialiseFloat(this->at(attr), 0.0f));
      return *this;
    }

    const CfgReader& withBool(const string& attr, function<void(const bool&)> func) const {
      if(this->has(attr))
        func(cms::deserialiseBool(this->at(attr), false));
      return *this;
    }

    const CfgReader& withVec2(const string& attr, function<void(const glm::vec2&)> func) const {
      if(this->has(attr))
        func(cms::deserialiseVec2(this->at(attr), glm::vec2(0.0f)));
      return *this;
    }

    const CfgReader& withVec3(const string& attr, function<void(const glm::vec3&)> func) const {
      if(this->has(attr))
        func(cms::deserialiseVec3(this->at(attr), glm::vec3(0.0f)));
      return *this;
    }

    const CfgReader& withVec4(const string& attr, function<void(const glm::vec4&)> func) const {
      if(this->has(attr))
        func(cms::deserialiseVec4(this->at(attr), glm::vec4(0.0f)));
      return *this;
    }

    const CfgReader& withColor(const string& attr, function<void(const ci::ColorAf&)> func) const {
      if(this->has(attr))
        func(cms::deserialiseColor(this->at(attr), ci::ColorAf(1.0f, 1.0f, 1.0f, 1.0f)));
      return *this;
    }

    const CfgReader& withRegex(const std::string& regex_str, function<void(const std::smatch& match, const std::string& v)> func) const {
      std::regex expr(regex_str);
      std::smatch match;

      for(auto it = this->begin(); it != this->end(); it++) {
        auto key = it->first;
        if( std::regex_match(it->first, match, expr) ) {
          func(match, it->second);
        }
      }

      return *this;
    }

  public: // conversion methods

    int getInt(const string& attr, int defaultValue) const {
      return has(attr) ? cms::deserialiseInt(this->at(attr), defaultValue) : defaultValue;
    }

    float getFloat(const string& attr, float defaultValue) const {
      return has(attr) ? cms::deserialiseFloat(this->at(attr), defaultValue) : defaultValue;
    }

    bool getBool(const string& attr, bool defaultValue) const {
      return has(attr) ? cms::deserialiseBool(this->at(attr), defaultValue) : defaultValue;
    }

    glm::vec2 getVec2(const string& attr, const glm::vec2& defaultValue) const {
      return has(attr) ? cms::deserialiseVec2(this->at(attr), defaultValue) : defaultValue;
    }

    glm::vec3 getVec3(const string& attr, const glm::vec3& defaultValue) const {
      return has(attr) ? cms::deserialiseVec3(this->at(attr), defaultValue) : defaultValue;
    }

    ci::ColorAf getColor(const string& attr, const ci::ColorAf& defaultValue) const {
      return has(attr) ? cms::deserialiseColor(this->at(attr), defaultValue) : defaultValue;
    }
  };
}}
