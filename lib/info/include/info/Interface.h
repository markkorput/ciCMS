#pragma once

#include "Port.hpp"
#include "Builder.hpp"
#include "Instance.h"

namespace info {

  class Interface {
    public:

      template<class T>
      static Interface* create(std::function<void(Builder<T>&)> func) {
        auto builder = new Builder<T>();
        auto interface = new Interface();

        // let caller configure our builder
        func(*builder);

        for(auto& builderOutput : builder->outputs) {
          auto output = std::shared_ptr<Port>(builderOutput->create());

          /// add output to the interface based on the output definitions added to the builder
          interface->outputs.push_back(output);

          /// add instance configuration logic to our interface based on definitions in the builder
          for(auto& func : builderOutput->applyFuncs) {
            interface->instanceFuncs.push_back([func, output](void* instance){
              func(instance, [output](const void* arg){
                output->invokeMethod(arg);
              });
            });
          }
        }

        return interface;
      }

      // configure an instance with the given Cfg
      template<class T>
      Instance& createInstance(T& instance) { 
        auto inst = new Instance();

        // for(auto& func : instanceFuncs)
          //   func((void*)&instance);

        return *inst;
      }

    public:
      const std::vector<std::shared_ptr<Port>>& getOutputs() const {
        return outputs;
      }

    private:
      std::vector<std::shared_ptr<Port>> outputs;
      std::vector<std::function<void(void*)>> instanceFuncs;
  };

}
