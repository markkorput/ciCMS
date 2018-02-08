#pragma once

#include "ctree/signal.hpp"
#include "ciCMS/State.h"

class Runner {
  public:
    void draw() {
      if(drawState.val())
        drawSignal.emit();
    }

  public:
    ctree::Signal<void()> drawSignal;
    cms::State<bool> drawState;
};
