#include <map>

#include "ctree/signal.hpp"
#include "ciCMS/ModelBase.h"
#include "ciCMS/State.h"

namespace cms { namespace cfg {
  class Cfg {

  public:
    Cfg() {
      this->signals = new map<string, void*>();
      this->bPrivateSignals = true;
      this->states = new map<string, void*>();
      this->bPrivateStates = true;
    }

    Cfg(const map<string, string> &data) {
      this->signals = new map<string, void*>();
      this->bPrivateSignals = true;
      this->states = new map<string, void*>();
      this->bPrivateStates = true;
      model.set(data);
    }

    Cfg(map<string, void*> &signals, map<string, void*> &states) {
      this->signals = &signals;
      this->bPrivateSignals = false;
      this->states = &states;
      this->bPrivateStates = false;
    }

    ~Cfg() {
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

    Cfg& set(const string& attr, string& var) { var = model.get(attr); return *this; }
    Cfg& set(const string& attr, int& var) { var = model.getInt(attr); return *this; }
    Cfg& set(const string& attr, bool& var) { var = model.getBool(attr); return *this; }


    // string get(string attr, string defaultVal="") { return model.get(attr, defaultVal); }

    template <typename Signature>
    Cfg& connect(const string& attr, std::function<Signature> func) {
      this->getSignal<Signature>(attr).connect(func);
      return *this;
    }

    // template <typename Signature>
    // Cfg& connect(const string& attr, const ctree::Signal<Signature> &sig) {
    //   getSignal<Signature>(attr).connect(&sig.emit);
    //   return *this;
    // }

  public:

    template <typename Signature>
    ::ctree::Signal<Signature>& getSignal(const std::string& id) {
      auto p = (*this->signals)[id];

      if (p != NULL) {
        return *(::ctree::Signal<Signature>*)p;
      }

      auto pp = new ::ctree::Signal<Signature>();
      (*this->signals)[id] = (void*)pp;
      return *pp;
    }

    template <typename Typ>
    cms::State<Typ>& getState(const string& id) {
      auto p = (*this->states)[id];

      if (p != NULL) return *(State<Typ>*)p;

      auto pp = new State<Typ>();
      (*this->states)[id] = (void*)pp;
      return *pp;
    }

  private:
    ModelBase model;
    bool bPrivateSignals = false, bPrivateStates = false;
    std::map<std::string, void*>* signals = NULL;
    std::map<std::string, void*>* states = NULL;
  };
}}
