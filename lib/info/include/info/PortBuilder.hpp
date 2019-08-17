#pragma once

#include "Port.hpp"

namespace info {

  class PortBuilderBase {
    public:
      PortBuilderBase(const std::string& id, const std::string& type, int flags) : id(id), type(type), flags(flags) {
      }
  
      Port* create() {
        return new Port(id, type, flags);
      }

      std::vector<std::function<void(void*, std::function<void(const void*)>)>> applyFuncs;
      std::string id;
      std::string type;
      int flags;
  };

  template<class T, typename V>
  class PortBuilder : protected PortBuilderBase {
    public:
      PortBuilder(const std::string& id, int flags) : PortBuilderBase(id, typeid(V).name(), flags) {}

    public:

      void apply(std::function<void(T&, std::function<void(const V&)>)> func) {
        // convert into void (typeless) function
        this->applyFuncs.push_back([func](void* instance, std::function<void(const void*)> voidvaloutfunc){
          func(*(T*)instance, [voidvaloutfunc](const V& val){
              voidvaloutfunc((void*)&val);
          });
        });
      }

      void apply(std::function<void(T&, TypedPort<V>&)> func) {
        
      }
  };
}