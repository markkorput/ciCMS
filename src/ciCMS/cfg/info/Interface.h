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

    public:
      void push(const T& val) {

      }
  };

  class Interface {
    public:
      // get/define an output
      template<typename T>
      Output<T>& output(const std::string& id) {
        auto output = new Output<T>(id);
        outputs.push_back(output);
        return *output;
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
      const std::vector<BaseOutput*>& getOutputs() const {
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
      std::vector<BaseOutput*> outputs;
      std::vector<std::function<void(void*)>> instanceFuncs;
  };

}}}
