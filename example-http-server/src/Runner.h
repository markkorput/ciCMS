#pragma once

// cinder
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"
// blocks
#include "ctree/signal.hpp"
#include "ciCMS/State.h"

class Runner {
  public:
    Runner() {
      drawState = true;
    }

    ~Runner() {
      destroy();
    }

    void setup() {
      setupSignal.emit();
      bExited = false;
    }

    void destroy() {
      if (!bExited) {
        exitSignal.emit();
        bExited = true;
      }
    }

    void draw() {
      if(drawState.val()) {
        drawSignal.emit();
      }
    }

    bool update() {
      if (bExited) return false;
      updateSignal.emit();
      return true;
    }

    void stop() {
      this->destroy(); // for now
    }

  public:
    ctree::Signal<void()> setupSignal, exitSignal, drawSignal, updateSignal;
    cms::State<bool> drawState;

  private:
    bool bExited=false;
};
