#pragma once

#include <map>

#include "boost/algorithm/string.hpp" // boost::is_any_of
#include "ctree/signal.hpp"
#include "ciCMS/ModelBase.h"
#include "ciCMS/State.h"
#include "ciCMS/cfg/CfgReader.hpp"

namespace cms { namespace cfg {
  class Cfg;
  typedef std::shared_ptr<Cfg> CfgRef;

  class Cfg {

  public:
    typedef std::function<void*(const std::string&)> ObjectFetcherFn;
    typedef std::function<void()> CompiledScriptFunc;

    struct ObjCallback {
      std::string id;
      std::function<void(void*)> func;
      ObjCallback(const std::string& _id, std::function<void(void*)> f) : id(_id), func(f) {}
    };

  public:
    Cfg();
    Cfg(const map<string, string> &data);
    Cfg(map<string, void*> &signals, map<string, void*> &states, ObjectFetcherFn objectFetcher = nullptr);

    ~Cfg();

    void setObjectFetcher(ObjectFetcherFn func){
      this->objectFetcher = func;
    }

    std::shared_ptr<CfgReader> reader() {
      return CfgReader::read(*this->attributes);
    }

    Cfg& set(const string& attr, string& var);
    Cfg& setInt(const string& attr, int& var);
    Cfg& setBool(const string& attr, bool& var);
    Cfg& setBool(const string& attr, State<bool>& var);
    Cfg& setFloat(const string& attr, float& var);
    Cfg& setVec2(const string& attr, glm::vec2& var);
    Cfg& set_ivec2(const string& attr, glm::ivec2& var);
    Cfg& setVec3(const string& attr, glm::vec3& var);
    Cfg& setVec4(const string& attr, glm::vec4& var);

    Cfg& setColor(const string& attr, ci::ColorAf& var);

    const std::map<std::string, std::string>* getAttributes() const { return this->attributes; }
    Cfg& setAttributes(const map<string, string> &data) { this->attributes = &data; return *this; }
    Cfg& withData(const map<string, string> &data) { this->attributes = &data; return *this; }

    template <typename Signature>
    Cfg& connect(const string& signalIds, std::function<Signature> func);

    template <typename Signature>
    Cfg& connectAttr(const string& attr, std::function<Signature> func);

    template <typename Typ>
    Cfg& push(const string& attr, Typ& var);

    template <typename Typ>
    Cfg& push(const string& attr, State<Typ>& targetState);

    template <typename Typ>
    Cfg& pushRef(const string& attr, Typ& var);

    template <typename Typ>
    Cfg& pushRef(const string& attr, State<Typ>& targetState);

    // template <typename Signature>
    // Cfg& connect(const string& attr, const ctree::Signal<Signature> &sig) {
    //   getSignal<Signature>(attr).connect(&sig.emit);
    //   return *this;
    // }

    //
    // get* (signal/state/object) methods
    //

    template <typename Signature>
    ::ctree::Signal<Signature>* getSignal(const std::string& id);

    template <typename Typ>
    cms::State<Typ>* getState(const string& id, const Typ* initialValue = NULL);

    void* getObjectPointer(const string& id);

    template<typename Typ>
    Typ* getObject(const string& id){
      return (Typ*)this->getObjectPointer(id);
    }

    template<typename ObjT>
    size_t getObjects(std::vector<ObjT*>& target, const std::string& ids, const std::string& delimiter=",");

    //
    // with* (signal/state/object) methods
    //

    template<typename ObjT>
    Cfg& withObject(const std::string& id, std::function<void(ObjT&)> func);

    template<typename ObjT>
    Cfg& withObjects(const std::string& ids, std::function<void(ObjT&)> func, std::string delimiter=",");

    template<typename ObjT>
    Cfg& withObjects(const std::string& ids, std::function<void(ObjT&, const std::string& objectId)> func, std::string delimiter=",");

    template<typename ObjT>
    Cfg& withObjectByAttr(const std::string& id, std::function<void(ObjT&)> func);

    template<typename ObjT>
    Cfg& withObjectsByAttr(const std::string& id, std::function<void(ObjT&)> func);

    template<typename Signature>
    Cfg& withSignalByAttr(const std::string& id, std::function<void(::ctree::Signal<Signature>&)> func);

    template<typename Typ>
    Cfg& withStateByAttr(const std::string& id, std::function<void(cms::State<Typ>&)> func);


    CompiledScriptFunc compileScript(const std::string& script);

    void notifyNewObject(const string& id, void* obj);

  private:
    const map<string, string>* attributes = NULL;

    bool bPrivateSignals = false, bPrivateStates = false;
    std::map<std::string, void*>* signals = NULL;
    std::map<std::string, void*>* states = NULL;

    vector<std::function<void()>> cleanupFuncs;
    ObjectFetcherFn objectFetcher = nullptr;

    // object callbacks
    std::vector<ObjCallback> objCallbacks;
  };

  template <typename Signature>
  Cfg& Cfg::connect(const string& signalIds, std::function<Signature> func) {
    // this->getSignal<Signature>(attr)->connect(func);
    std::vector<std::string> ids;
    boost::split(ids, signalIds, boost::is_any_of(","));
    for(auto& id : ids)
      this->getSignal<Signature>(id)->connect(func);

    return *this;
  }

  template <typename Signature>
  Cfg& Cfg::connectAttr(const string& attr, std::function<Signature> func) {
    auto readr = reader();

    if (readr->has(attr)) {
      // this->getSignal<Signature>(readr->get(attr))->connect(func);
      std::string value = readr->get(attr);
      std::vector<std::string> ids;  
      boost::split(ids, value, boost::is_any_of(","));

      for(auto& id : ids)
        this->getSignal<Signature>(id)->connect(func);
    }

    return *this;
  }

  // template <typename Signature>
  // Cfg& connect(const string& attr, const ctree::Signal<Signature> &sig) {
  //   getSignal<Signature>(attr).connect(&sig.emit);
  //   return *this;
  // }

  template <typename Typ>
  Cfg& Cfg::push(const string& attr, Typ& var) {
    auto state = this->getState<Typ>(attr, &var);
    state->push([&var](const Typ& val){ var = val; });
    return *this;
  }

  template <typename Typ>
  Cfg& Cfg::push(const string& attr, State<Typ>& targetState) {
    auto existingVal = targetState.val();
    auto state = this->getState<Typ>(attr, &existingVal);
    state->push(targetState);
    return *this;
  }

  template <typename Typ>
  Cfg& Cfg::pushRef(const string& attr, Typ& var) {
    auto readr = reader();
    if (readr->has(attr)) this->push(readr->get(attr), var);
    return *this;
  }

  template <typename Typ>
  Cfg& Cfg::pushRef(const string& attr, State<Typ>& targetState) {
    auto readr = reader();
    if (readr->has(attr)) this->push(readr->get(attr), targetState);
    return *this;
  }

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
  cms::State<Typ>* Cfg::getState(const string& id, const Typ* initialValue) {
    // look for existing state
    auto p = (*this->states)[id];
    // return existing state
    if (p != NULL) return (State<Typ>*)p;
    // create new state
    auto pp = new State<Typ>();
    // set initial value
    if (initialValue != NULL) pp->set(*initialValue);
    // save new state in our states map
    (*this->states)[id] = (void*)pp;
    // make sure the state is removed by our destructor
    cleanupFuncs.push_back([this, id](){ delete (State<Typ>*)(*this->states)[id]; this->states->erase(id); });
    // return new state
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
  Cfg& Cfg::withObject(const std::string& id, std::function<void(ObjT&)> func) {
    auto p = this->getObject<ObjT>(id);

    if(p) {
      func(*p);
      return *this;
    }

    // register callback to get invoked when object appears
    this->objCallbacks.push_back(ObjCallback(id, [id, func](void* objPointer) {
      func(*(ObjT*)objPointer);
    }));

    return *this;
  }

  template<typename ObjT>
  Cfg& Cfg::withObjects(const std::string& ids, std::function<void(ObjT&)> func, std::string delimiter) {
    std::vector<std::string> strings;
    boost::split(strings, ids, boost::is_any_of(delimiter));

    for(auto& id : strings) {
      this->withObject<ObjT>(id, func);
    }

    return *this;
  }

  template<typename ObjT>
  Cfg& Cfg::withObjects(const std::string& ids, std::function<void(ObjT&, const std::string& objectId)> func, std::string delimiter) {
    std::vector<std::string> strings;
    boost::split(strings, ids, boost::is_any_of(delimiter));

    for(auto& id : strings) {
      this->withObject<ObjT>(id, [func, id](ObjT& obj) {
        func(obj, id);
      });
    }

    return *this;
  }

  template<typename ObjT>
  Cfg& Cfg::withObjectByAttr(const std::string& attr, std::function<void(ObjT&)> func) {
    auto reader = CfgReader::read(*this->attributes);
    reader->with(attr, [this, func](const std::string& val){
      this->withObject<ObjT>(val, func);
    });

    return *this;
  }

  template<typename ObjT>
  Cfg& Cfg::withObjectsByAttr(const std::string& id, std::function<void(ObjT&)> func) {
    auto reader = CfgReader::read(*this->attributes);
    reader->with(id, [this, func](const std::string& val){
      this->withObjects<ObjT>(val, func);
    });

    return *this;
  }

  template<typename Signature>
  Cfg& Cfg::withSignalByAttr(const std::string& id, std::function<void(::ctree::Signal<Signature>&)> func) {
    auto reader = CfgReader::read(*this->attributes);

    reader->with(id, [this, func](const std::string& signalId){
      auto pSignal = this->getSignal<Signature>(signalId);
      func(*pSignal);
    });

    return *this;
  }

  template<typename Typ>
  Cfg& Cfg::withStateByAttr(const std::string& id, std::function<void(cms::State<Typ>&)> func) {
    auto reader = CfgReader::read(*this->attributes);

    reader->with(id, [this, func](const std::string& stateId){
      auto pState = this->getState<Typ>(stateId);
      func(*pState);
    });

    return *this;
  }
}}
