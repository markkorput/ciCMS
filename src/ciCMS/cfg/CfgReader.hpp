#include <map>
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

    const CfgReader& withInt(const string& attr, function<void(const int&)> func){
      if(this->has(attr))
        func(cms::deserialiseInt(this->at(attr), 0));
      return *this;
    }

    const CfgReader& withFloat(const string& attr, function<void(float)> func){
      if(this->has(attr))
        func(cms::deserialiseFloat(this->at(attr), 0.0f));
      return *this;
    }

    const CfgReader& withBool(const string& attr, function<void(const bool&)> func){
      if(this->has(attr))
        func(cms::deserialiseBool(this->at(attr), false));
      return *this;
    }

    const CfgReader& withVec2(const string& attr, function<void(const glm::vec2&)> func){
      if(this->has(attr))
        func(cms::deserialiseVec2(this->at(attr), glm::vec2(0.0f)));
      return *this;
    }

    const CfgReader& withVec3(const string& attr, function<void(const glm::vec3&)> func){
      if(this->has(attr))
        func(cms::deserialiseVec3(this->at(attr), glm::vec3(0.0f)));
      return *this;
    }

    const CfgReader& withColor(const string& attr, function<void(const ci::ColorAf&)> func){
      if(this->has(attr))
        func(cms::deserialiseColor(this->at(attr), ci::ColorAf(1.0f, 1.0f, 1.0f, 1.0f)));
      return *this;
    }

  public: // conversion methods

    int getInt(const string& attr, int defaultValue){
      return cms::deserialiseInt(this->at(attr), defaultValue);
    }

    float getFloat(const string& attr, float defaultValue){
      return cms::deserialiseFloat(this->at(attr), defaultValue);
    }

    bool getBool(const string& attr, bool defaultValue){
      return cms::deserialiseBool(this->at(attr), defaultValue);
    }

    glm::vec2 getVec2(const string& attr, const glm::vec2& defaultValue){
      return cms::deserialiseVec2(this->at(attr), defaultValue);
    }

    glm::vec3 getVec3(const string& attr, const glm::vec3& defaultValue){
      return cms::deserialiseVec3(this->at(attr), defaultValue);
    }

    ci::ColorAf getColor(const string& attr, const ci::ColorAf& defaultValue){
      return cms::deserialiseColor(this->at(attr), defaultValue);
    }
  };
}}
