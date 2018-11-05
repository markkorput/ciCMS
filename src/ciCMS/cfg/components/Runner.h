#pragma once

#include "ctree/signal.hpp"
#include "ciCMS/cfg/Cfg.h"

namespace cms { namespace cfg { namespace components {
  class Runner {
    public: // methods
      bool update();
      void draw();
      void cfg(cms::cfg::Cfg& cfg);

    private: // attributes
      bool bDone = false;
      ::ctree::Signal<void()> updateSignal;
      ::ctree::Signal<void()> drawSignal;
  };
}}}
