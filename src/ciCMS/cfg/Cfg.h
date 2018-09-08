#include <map>

#include "boost/algorithm/string.hpp" // boost::is_any_of
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

    template<typename ObjT>
    size_t getObjects(std::vector<ObjT*>& target, const std::string& ids, const std::string& delimiter=",");

    template<typename ObjT>
    void withObject(const std::string& id, std::function<void(ObjT&)> func);

    template<typename ObjT>
    void withObjects(const std::string& ids, std::function<void(ObjT&)> func, std::string delimiter=",");

    template<typename ObjT>
    void withObjects(const std::string& ids, std::function<void(ObjT&, const std::string& objectId)> func, std::string delimiter=",");

    CompiledScriptFunc compileScript(const std::string& script);

  private:
    ModelBase model;

    bool bPrivateSignals = false, bPrivateStates = false;
    std::map<std::string, void*>* signals = NULL;
    std::map<std::string, void*>* states = NULL;

    vector<std::function<void()>> cleanupFuncs;
    ObjectFetcherFn objectFetcher = nullptr;
  };


  // string get(string attr, string defaultVal="") { return model.get(attr, defaultVal); }

  template <typename Signature>
  Cfg& Cfg::connect(const string& attr, std::function<Signature> func) {
    this->getSignal<Signature>(attr)->connect(func);
    return *this;
  }

  // template <typename Signature>
  // Cfg& connect(const string& attr, const ctree::Signal<Signature> &sig) {
  //   getSignal<Signature>(attr).connect(&sig.emit);
  //   return *this;
  // }


  template <typename Signature>
  ::ctree::Signal<Signature>* Cfg::getSignal(const std::string& id) {
    auto p = (*this->signals)[id];

    if (p != NULL) return (::ctree::Signal<Signature>*)p;

    auto pp = new ::ctree::Signal<Signature>();
    (*this->signals)[id] = (void*)pp;
    cleanupFuncs.push_back([this, id](){ delete (::ctree::Signal<Signature>*)(*this->signals)[id]; this->signals->erase(id); });
    return pp;
  }

  template <typename Typ>
  cms::State<Typ>* Cfg::getState(const string& id) {
    auto p = (*this->states)[id];

    if (p != NULL) return (State<Typ>*)p;

    auto pp = new State<Typ>();
    (*this->states)[id] = (void*)pp;
    cleanupFuncs.push_back([this, id](){ delete (State<Typ>*)(*this->states)[id]; this->states->erase(id); });
    return pp;
  }

  template<typename ObjT>
  size_t Cfg::getObjects(std::vector<ObjT*>& target, const std::string& ids, const std::string& delimiter) {
    std::vector<std::string> strings;
    boost::split(strings, ids, boost::is_any_of(delimiter));

    size_t count=0;
    for(auto& id : strings) {
      auto p = (ObjT*)this->getObjectPointer(id);
      if (p) {
        target.push_back(p);
        count += 1;
      }
    }

    return count;
  }

  template<typename ObjT>
  void Cfg::withObject(const std::string& id, std::function<void(ObjT&)> func) {
    auto p = this->getObject<ObjT>(id);

    if(p) {
      func(*p);
      return;
    }

    // register callback to get invoked later

    // ObjCallback oc;
    // oc.id = id;
    // oc.func = [func](void* objPointer) {
    //   func(*(ObjT*)objPointer);
    // };

    // this->objCallbacks.push_back(oc);
  }

  template<typename ObjT>
  void Cfg::withObjects(const std::string& ids, std::function<void(ObjT&)> func, std::string delimiter) {
    std::vector<std::string> strings;
    boost::split(strings, ids, boost::is_any_of(delimiter));

    for(auto& id : strings) {
      this->withObject<ObjT>(id, func);
    }
  }

  template<typename ObjT>
  void Cfg::withObjects(const std::string& ids, std::function<void(ObjT&, const std::string& objectId)> func, std::string delimiter) {
    std::vector<std::string> strings;
    boost::split(strings, ids, boost::is_any_of(delimiter));

    for(auto& id : strings) {
      this->withObject<ObjT>(id, [func, id](ObjT& obj) {
        func(obj, id);
      });
    }
  }

}}
