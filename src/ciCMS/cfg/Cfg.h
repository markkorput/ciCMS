#include <map>

#include "ctree/signal.hpp"
#include "ciCMS/ModelBase.h"
#include "ciCMS/State.h"

namespace cms { namespace cfg {
  class Cfg {

  public:
    typedef std::function<void*(const std::string&)> ObjectFetcherFn;
    typedef std::function<void()> CompiledScriptFunc;

  public:
    Cfg();
    Cfg(const map<string, string> &data);
    Cfg(map<string, void*> &signals, map<string, void*> &states, ObjectFetcherFn objectFetcher = nullptr);

    ~Cfg();

    Cfg& set(const string& attr, string& var);
    Cfg& set(const string& attr, int& var);
    Cfg& set(const string& attr, bool& var);

    // string get(string attr, string defaultVal="") { return model.get(attr, defaultVal); }

    template <typename Signature>
    Cfg& connect(const string& attr, std::function<Signature> func);

    // template <typename Signature>
    // Cfg& connect(const string& attr, const ctree::Signal<Signature> &sig) {
    //   getSignal<Signature>(attr).connect(&sig.emit);
    //   return *this;
    // }

    template <typename Signature>
    ::ctree::Signal<Signature>* getSignal(const std::string& id);

    template <typename Typ>
    cms::State<Typ>* getState(const string& id);

    void* getObjectPointer(const string& id);

    template<typename Typ>
    Typ* getObject(const string& id){
      return (Typ*)this->getObjectPointer(id);
    }

    CompiledScriptFunc compileScript(const std::string& script);

  private:
    ModelBase model;

    bool bPrivateSignals = false, bPrivateStates = false;
    std::map<std::string, void*>* signals = NULL;
    std::map<std::string, void*>* states = NULL;

    ObjectFetcherFn objectFetcher = nullptr;
  };


  // string get(string attr, string defaultVal="") { return model.get(attr, defaultVal); }

  template <typename Signature>
  Cfg& cms::cfg::Cfg::connect(const string& attr, std::function<Signature> func) {
    this->getSignal<Signature>(attr)->connect(func);
    return *this;
  }

  // template <typename Signature>
  // Cfg& connect(const string& attr, const ctree::Signal<Signature> &sig) {
  //   getSignal<Signature>(attr).connect(&sig.emit);
  //   return *this;
  // }


  template <typename Signature>
  ::ctree::Signal<Signature>* cms::cfg::Cfg::getSignal(const std::string& id) {
    auto p = (*this->signals)[id];

    if (p != NULL) return (::ctree::Signal<Signature>*)p;

    auto pp = new ::ctree::Signal<Signature>();
    (*this->signals)[id] = (void*)pp;
    return pp;
  }

  template <typename Typ>
  cms::State<Typ>* cms::cfg::Cfg::getState(const string& id) {
    auto p = (*this->states)[id];

    if (p != NULL) return (State<Typ>*)p;

    auto pp = new State<Typ>();
    (*this->states)[id] = (void*)pp;
    return pp;
  }

}}
