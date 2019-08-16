#pragma once

#include "Port.hpp"

namespace cms { namespace cfg { namespace info {

  template<typename T>
  class Builder {

    class BuilderBasePort {
      public:
        BuilderBasePort(const std::string& id, const std::string& type, int flags) : id(id), type(type), flags(flags) {
        }
    
        Port* create() {
          return new Port(id, type, flags);
        }

        std::vector<std::function<void(void*, std::function<void(const void*)>)>> applyFuncs;
        std::string id;
        std::string type;
        int flags;
    };

    template<typename V>
    class BuilderPort : protected BuilderBasePort {
      public:
        BuilderPort(const std::string& id, int flags) : BuilderBasePort(id, typeid(V).name(), flags) {}

      public:

        void apply(std::function<void(T&, std::function<void(const V&)>)> func) {
          // convert into void (typeless) function
          this->applyFuncs.push_back([func](void* instance, std::function<void(const void*)> voidvaloutfunc){
            func(*(T*)instance, [voidvaloutfunc](const V& val){
                voidvaloutfunc((void*)&val);
            });
          });
        }
    };

    public:

      template<typename V>
      BuilderPort<V>& attr(const std::string& id) { return addPort<V>(id, Port::FLAG_INOUT); }

      template<typename V>
      BuilderPort<V>& output(const std::string& id) { return addPort<V>(id, Port::FLAG_OUT); }

      template<typename V>
      BuilderPort<V>& addPort(const std::string& id, int flags) {
        auto output = new BuilderPort<V>(id, flags);
        outputs.push_back((BuilderBasePort*)output);
        return *output;
      }

    public:
      std::vector<BuilderBasePort*> outputs;
  };
}}}