#include <map>
#include <regex>
#include "ciCMS/deserialise.h"

namespace cms { namespace cfg {
  class CfgReader : public std::map<std::string, std::string> {
  public:
    static std::shared_ptr<CfgReader> read(const std::map<std::string, std::string>& data) {
      return std::make_shared<CfgReader>(data);
    }

    CfgReader(const std::map<std::string, std::string>& attributes) : attrs(&attributes) {}

    bool has(const string& attr) const {
      return (attrs->find(attr) != attrs->end());
    }

  public: // "with" methods

    const CfgReader& with(const string& attr, function<void(const string&)> func) const {
      if(this->has(attr))
        func(attrs->at(attr));
      return *this;
    }

    const CfgReader& withInt(const string& attr, function<void(int)> func) const {
      if(this->has(attr))
        func(cms::deserialiseInt(attrs->at(attr), 0));
      return *this;
    }

    const CfgReader& withFloat(const string& attr, function<void(float)> func) const {
      if(this->has(attr))
        func(cms::deserialiseFloat(attrs->at(attr), 0.0f));
      return *this;
    }

    const CfgReader& withBool(const string& attr, function<void(bool)> func) const {
      if(this->has(attr))
        func(cms::deserialiseBool(attrs->at(attr), false));
      return *this;
    }

    const CfgReader& withVec2(const string& attr, function<void(const glm::vec2&)> func) const {
      if(this->has(attr))
        func(cms::deserialiseVec2(attrs->at(attr), glm::vec2(0.0f)));
      return *this;
    }

    const CfgReader& withVec3(const string& attr, function<void(const glm::vec3&)> func) const {
      if(this->has(attr))
        func(cms::deserialiseVec3(attrs->at(attr), glm::vec3(0.0f)));
      return *this;
    }

    const CfgReader& withColor(const string& attr, function<void(const ci::ColorAf&)> func) const {
      if(this->has(attr))
        func(cms::deserialiseColor(attrs->at(attr), ci::ColorAf(1.0f, 1.0f, 1.0f, 1.0f)));
      return *this;
    }

    const CfgReader& withRegex(const std::string& regex_str, function<void(const std::smatch& match, const std::string& v)> func) const {
      std::regex expr(regex_str);
      std::smatch match;

      for(auto it = attrs->begin(); it != attrs->end(); it++) {
        auto key = it->first;
        if( std::regex_match(it->first, match, expr) ) {
          func(match, it->second);
        }
      }

      return *this;
    }

  public: // conversion methods

    std::string get(const string& attr, string defaultValue="") const {
      return has(attr) ? attrs->at(attr) : defaultValue;
    }

    int getInt(const string& attr, int defaultValue) const {
      return has(attr) ? cms::deserialiseInt(attrs->at(attr), defaultValue) : defaultValue;
    }

    float getFloat(const string& attr, float defaultValue) const {
      return has(attr) ? cms::deserialiseFloat(attrs->at(attr), defaultValue) : defaultValue;
    }

    bool getBool(const string& attr, bool defaultValue) const {
      return has(attr) ? cms::deserialiseBool(attrs->at(attr), defaultValue) : defaultValue;
    }

    glm::vec2 getVec2(const string& attr, const glm::vec2& defaultValue) const {
      return has(attr) ? cms::deserialiseVec2(attrs->at(attr), defaultValue) : defaultValue;
    }

    glm::vec3 getVec3(const string& attr, const glm::vec3& defaultValue) const {
      return has(attr) ? cms::deserialiseVec3(attrs->at(attr), defaultValue) : defaultValue;
    }

    ci::ColorAf getColor(const string& attr, const ci::ColorAf& defaultValue) const {
      return has(attr) ? cms::deserialiseColor(attrs->at(attr), defaultValue) : defaultValue;
    }

  private:
    const std::map<std::string, std::string> *attrs;
  };
}}
