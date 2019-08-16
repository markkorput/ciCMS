#pragma once

#include "ctree/signal.hpp"
#include "ciCMS/cfg/Cfg.h"

namespace component {
  class Runner {
    public: // methods
      void draw();
      void cfg(cms::cfg::Cfg& cfg);

    private: // attributes
      bool bDrawEnabled = true;
      ctree::Signal<void()> drawSignal;
  };
}
