#include "Cfg.h"

using namespace cms::cfg;

cms::cfg::Cfg::Cfg() {
  this->signals = new map<string, void*>();
  this->bPrivateSignals = true;
  this->states = new map<string, void*>();
  this->bPrivateStates = true;
}

cms::cfg::Cfg::Cfg(const map<string, string> &data) {
  this->signals = new map<string, void*>();
  this->bPrivateSignals = true;
  this->states = new map<string, void*>();
  this->bPrivateStates = true;
  model.set(data);
}

cms::cfg::Cfg::Cfg(map<string, void*> &signals, map<string, void*> &states) {
  this->signals = &signals;
  this->bPrivateSignals = false;
  this->states = &states;
  this->bPrivateStates = false;
}

cms::cfg::Cfg::~Cfg() {
  if (signals && bPrivateSignals) {
    // for (auto it = signals.begin(); it != signals.end(); it++)
    //   delete it->second;
    std::cout << "[cms::cfg::Cfg] TODO: deallocate all singals in" << std::endl;
    delete signals;
  }

  signals = NULL;

  if (states && bPrivateStates) {
    // for (auto it = states.begin(); it != states.end(); it++)
    //   delete it->second;
    std::cout << "[cms::cfg::Cfg] TODO: deallocate all states in" << std::endl;
    delete states;
  }
  states = NULL;
}

Cfg& cms::cfg::Cfg::set(const string& attr, string& var) { var = model.get(attr); return *this; }
Cfg& cms::cfg::Cfg::set(const string& attr, int& var) { var = model.getInt(attr); return *this; }
Cfg& cms::cfg::Cfg::set(const string& attr, bool& var) { var = model.getBool(attr); return *this; }
