#pragma once

#include "ctree/signal.hpp"
#include "ciCMS/cfg/Cfg.h"

namespace cms { namespace cfg { namespace info {

  class BaseOutput {
    public:
      BaseOutput(const std::string& id, const std::string& type) : id(id), type(type) {
      }
  
    public:
      const std::string& getId() const { return id; }
      const std::string& getType() const { return type; }

      void invokeMethod(const void* arg){
        signal.emit();
      }

    private:
      std::string id;
      std::string type;
      ::ctree::Signal<void(void)> signal;
  };

  // template<typename T>
  // class Output : public BaseOutput {
  //   public:
  //     Output(const std::string& id) : BaseOutput(id, typeid(T).name()) {}
  // };

}}}
