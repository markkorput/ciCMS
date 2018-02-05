#pragma once

#include "ctree/signal.hpp"

class Runner {
  public:
    void draw() {
      drawSignal.emit();
    }

  public:
    ctree::Signal<void()> drawSignal;
};
