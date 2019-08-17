#pragma once

#include "ctree/signal.hpp"
#include "ciCMS/cfg/Cfg.h"

namespace cms { namespace cfg { namespace info {

  class Port {
    public:
      const static int FLAG_IN = 1;
      const static int FLAG_OUT = (1 >> 1);
      const static int FLAG_INOUT = (FLAG_IN & FLAG_OUT);

    public:

      Port(const std::string& id, const std::string& type, int flags = FLAG_INOUT) : id(id), type(type), flags(flags) {
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
      int flags;
      ::ctree::Signal<void(void)> signal;
  };

  template<typename V>
  class TypedPort : public Port {
  };

}}}
