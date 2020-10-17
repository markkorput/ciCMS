#pragma once

#include "ciCMS/cfg/Cfg.h"
#include "Base.h"

namespace cms { namespace cfg { namespace components {

  class Script : public Base {
    public:
      void cfg(cms::cfg::Cfg& cfg);

    private:
      std::function<void()> func = nullptr;
  };

}}}
