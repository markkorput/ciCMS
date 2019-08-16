#pragma once

#include "ciCMS/cfg/Cfg.h"

namespace cms { namespace cfg { namespace info {

  class BaseOutput {
    public:
      BaseOutput(const std::string& id) : id(id) {
      }
  

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

    private:
      std::vector<BaseOutput*> outputs;
  };
}}}
