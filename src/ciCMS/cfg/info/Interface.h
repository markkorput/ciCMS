#pragma once

#include "ciCMS/cfg/Cfg.h"

namespace cms { namespace cfg { namespace info {

  class BaseOutput {
    public:
      BaseOutput(const std::string& id) : id(id) {
      }
  
    public:
      const std::string& getId() const { return id; }

    private:
      std::string id;
  };

  template<typename T>
  class Output : public BaseOutput {
    public:
      Output(const std::string& id) : BaseOutput(id) {}

    public:
      void push(const T& val) {

      }
  };

  class Interface {
    public:
      void cfg(cms::cfg::Cfg& cfg){}

      template<typename T>
      Output<T>& output(const std::string& id) {
        auto output = new Output<T>(id);
        outputs.push_back(output);
        return *output;
      }

    public:
      const std::vector<BaseOutput*>& getOutputs() const {
        return outputs;
      }

    private:
      std::vector<BaseOutput*> outputs;
  };
}}}
