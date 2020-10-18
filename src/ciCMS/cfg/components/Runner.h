#pragma once

// cinder
#include "cinder/gl/gl.h"
// blocks
#include "ctree/signal.hpp"
#include "ciCMS/State.h"
#include "ciCMS/cfg/Cfg.h"
#include "Base.h"

namespace cms { namespace cfg { namespace components {
  class Runner : public Base {

    public:
      void setup();
      void exit();
      void draw();
      void update();
      void notifyAboutConfig();

      void cfg(cfg::Cfg& cfg);

    public:
      ::ctree::Signal<void()>
        setupSignal,
        exitSignal,
        drawSignal,
        updateSignal,
        configSignal;

    private:
      ci::vec3 mScale=ci::vec3(1.0);
      ci::vec3 mTranslate=ci::vec3(0.0);
  };
}}}
