#pragma once

#include "Output.hpp"

namespace cms { namespace cfg { namespace info {

  template<typename T>
  class Builder {

    class BuilderBaseOutput {
      public:
        BuilderBaseOutput(const std::string& id, const std::string& type) : id(id), type(type) {
        }
    
        BaseOutput* create() {
          return new BaseOutput(id, type);
        }

        std::vector<std::function<void(void*, std::function<void(const void*)>)>> applyFuncs;
        std::string id;
        std::string type;
    };

    template<typename V>
    class BuilderOutput : protected BuilderBaseOutput {
      public:
        BuilderOutput(const std::string& id) : BuilderBaseOutput(id, typeid(V).name()) {}

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
      BuilderOutput<V>& output(const std::string& id) {
        auto output = new BuilderOutput<V>(id);
        outputs.push_back((BuilderBaseOutput*)output);

        return *output;
      }

    public:
      std::vector<BuilderBaseOutput*> outputs;
  };
}}}