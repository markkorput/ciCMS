#pragma once

#include "ciCMS/cfg/Cfg.h"

namespace cms { namespace cfg { namespace info {

  class BaseOutput {
    public:
      BaseOutput(const std::string& id, const std::string& type) : id(id), type(type) {
      }
  
    public:
      const std::string& getId() const { return id; }
      const std::string& getType() const { return type; }

    private:
      std::string id;
      std::string type;
  };

  template<typename T>
  class Output : public BaseOutput {
    public:
      Output(const std::string& id) : BaseOutput(id, typeid(T).name()) {}
  };

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

  class Interface {
    public:

      template<class T>
      static cfg::info::Interface* create(std::function<void(Builder<T>&)> func) {
        auto builder = new Builder<T>();
        auto interface = new Interface();

        func(*builder);

        for(auto& builderOutput : builder->outputs) {
          interface->outputs.push_back(std::shared_ptr<BaseOutput>(builderOutput->create()));
        }

        return interface;
      }

      // configure an instance with the given Cfg
      template<class T>
      void configureInstance(T& instance) { 
        for(auto& func : instanceFuncs)
          func((void*)&instance);
      }

      void cfg(cms::cfg::Cfg& cfg) {

      }

    public:
      const std::vector<std::shared_ptr<BaseOutput>>& getOutputs() const {
        return outputs;
      }

    public:
      template<class T>
      void withInstance(std::function<void(T&)> func) {
        instanceFuncs.push_back([func](void* t){
          func(*(T*)t);
        });
      }

    private:
      std::vector<std::shared_ptr<BaseOutput>> outputs;
      std::vector<std::function<void(void*)>> instanceFuncs;
  };

}}}
