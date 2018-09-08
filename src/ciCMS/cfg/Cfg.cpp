#include "Cfg.h"
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

Cfg::Cfg(const map<string, string> &data) {
  this->signals = new map<string, void*>();
  this->bPrivateSignals = true;
  this->states = new map<string, void*>();
  this->bPrivateStates = true;
  model.set(data);
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
    // for (auto it = signals.begin(); it != signals.end(); it++)
    //   delete it->second;
    // std::cout << "[cms::cfg::Cfg] TODO: deallocate all singals in" << std::endl;
    delete signals;
  }

  signals = NULL;

  if (states && bPrivateStates) {
    // for (auto it = states.begin(); it != states.end(); it++)
    //   delete it->second;
    // std::cout << "[cms::cfg::Cfg] TODO: deallocate all states in" << std::endl;
    delete states;
  }
  states = NULL;
}

Cfg& Cfg::set(const string& attr, string& var) { var = model.get(attr); return *this; }
Cfg& Cfg::set(const string& attr, int& var) { var = model.getInt(attr); return *this; }
Cfg& Cfg::set(const string& attr, bool& var) { var = model.getBool(attr); return *this; }

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
