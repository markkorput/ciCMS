#include "Cfg.h"
#include "CfgReader.hpp"
#include "ciCMS/deserialise.h"


#include <regex>
#include "boost/algorithm/string.hpp" // boost::is_any_of

using namespace cms::cfg;

Cfg::Cfg() {
  this->signals = new map<string, void*>();
  this->bPrivateSignals = true;
  this->states = new map<string, void*>();
  this->bPrivateStates = true;
}

Cfg::Cfg(const map<string, string> &data) : attributes(&data) {
  this->signals = new map<string, void*>();
  this->bPrivateSignals = true;
  this->states = new map<string, void*>();
  this->bPrivateStates = true;
}

Cfg::Cfg(map<string, void*> &signals, map<string, void*> &states, ObjectFetcherFn objectFetcher ) {
  this->signals = &signals;
  this->bPrivateSignals = false;
  this->states = &states;
  this->bPrivateStates = false;
  this->objectFetcher = objectFetcher;
}

Cfg::~Cfg() {
  for(auto fn : cleanupFuncs) fn();
  cleanupFuncs.clear();

  if (signals && bPrivateSignals) {
    delete signals;
  }

  signals = NULL;

  if (states && bPrivateStates) {
    delete states;
  }
  states = NULL;
}

Cfg& Cfg::set(const string& attr, string& var) {
  if (this->attributes != NULL) {
    auto reader = CfgReader::read(*this->attributes);
    if (reader->has(attr)) var = reader->get(attr, "");
  }
  return *this; }

Cfg& Cfg::setInt(const string& attr, int& var) {
  if (this->attributes != NULL) {
    auto reader = CfgReader::read(*this->attributes);
    if (reader->has(attr)) var = reader->getInt(attr, 0);
  }
  return *this; }

Cfg& Cfg::setBool(const string& attr, bool& var) {
  if (this->attributes != NULL) {
    auto reader = CfgReader::read(*this->attributes);
    if (reader->has(attr)) var = reader->getBool(attr, false);
  }
  return *this; }

Cfg& Cfg::setFloat(const string& attr, float& var) {
  if (this->attributes != NULL) {
    auto reader = CfgReader::read(*this->attributes);
    if (reader->has(attr)) var = reader->getFloat(attr, 0.0f);
  }
  return *this; }

Cfg& Cfg::setVec2(const string& attr, glm::vec2& var) {
  if (this->attributes != NULL) {
    auto reader = CfgReader::read(*this->attributes);
    if (reader->has(attr)) var = reader->getVec2(attr, glm::vec2(0.0f));
  }
  return *this;
}

Cfg& Cfg::setVec3(const string& attr, glm::vec3& var) {
  if (this->attributes != NULL) {
    auto reader = CfgReader::read(*this->attributes);
    if (reader->has(attr)) var = reader->getVec3(attr, glm::vec3(0.0f));
  }
  return *this;
}

Cfg& Cfg::setColor(const string& attr, ci::ColorA& var) {
  if (this->attributes != NULL) {
    auto reader = CfgReader::read(*this->attributes);
    if (reader->has(attr)) var = reader->getColor(attr, var);
  }

  return *this;
}

void* Cfg::getObjectPointer(const string& id) {
  return this->objectFetcher ? this->objectFetcher(id) : NULL;
}

Cfg::CompiledScriptFunc Cfg::compileScript(const std::string& script) {
  std::vector<std::string> scripts;
  boost::split(scripts, script, boost::is_any_of(";"));

  auto funcRefs = std::make_shared<std::vector<CompiledScriptFunc>>();
  std::smatch match;

  for(auto& src : scripts) {
    { std::regex expr("^emit:(\\w+)$");
      if( std::regex_match(src, match, expr) ) {
        auto pSignal = this->getSignal<void()>(match[1]);
        funcRefs->push_back( [pSignal](){ pSignal->emit(); } );
      }
    }

    { std::regex expr("^toggle:(\\w+)$");
      if( std::regex_match(src, match, expr) ) {
        auto pState = this->getState<bool>(match[1]);
        funcRefs->push_back( [pState](){
          pState->operator=(!pState->val());
        } );
      }
    }

    { std::regex expr("^\\+(\\d+):(\\w+)$");
      if( std::regex_match(src, match, expr) ) {
        int delta = cms::deserialiseInt(match[1], 0);
        auto pState = this->getState<int>(match[2]);

        funcRefs->push_back( [pState, delta](){
          pState->operator=(pState->val()+delta);
        } );
      }
    }

    { std::regex expr("^\\-(\\d+):(\\w+)$");
      if( std::regex_match(src, match, expr) ) {
        int delta = cms::deserialiseInt(match[1], 0);
        auto pState = this->getState<int>(match[2]);

        funcRefs->push_back( [pState, delta](){
          pState->operator=(pState->val()-delta);
        } );
      }
    }

    { std::regex expr("^\\+(\\d+\\.\\d)+:(\\w+)$");
      if( std::regex_match(src, match, expr) ) {
        auto delta = cms::deserialiseFloat(match[1], 0.0f);
        auto pState = this->getState<float>(match[2]);

        funcRefs->push_back( [pState, delta](){
          pState->operator=(pState->val()+delta);
        } );
      }
    }

    { std::regex expr("^\\-(\\d+\\.\\d)+:(\\w+)$");
      if( std::regex_match(src, match, expr) ) {
        auto delta = cms::deserialiseFloat(match[1], 0.0f);
        auto pState = this->getState<float>(match[2]);

        funcRefs->push_back( [pState, delta](){
          pState->operator=(pState->val()-delta);
        } );
      }
    }

    // return [](){};
  }

  return funcRefs->size() == 1 ? funcRefs->at(0) : [funcRefs]() {
    for(auto func : (*funcRefs)) { func(); }
  };
}

void Cfg::notifyNewObject(const string& id, void* obj) {
  for(int i=this->objCallbacks.size()-1; i>=0; i--) {
    auto oc = this->objCallbacks[i];
    if(oc.id == id) {
      oc.func(obj);
      this->objCallbacks.erase(objCallbacks.begin()+i);
    }
  }
}
