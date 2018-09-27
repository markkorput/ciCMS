#pragma once

#include "ciCMS/cfg/Cfg.h"
#include "ctree/signal.hpp"
#include "cinder/Signals.h"
#include "cinder/CameraUi.h"

namespace component {
  class Camera {

    public:
      void cfg(cms::cfg::Cfg& cfg);

    private:
      void render();

    private:
      ci::CameraPersp cam;
      ci::signals::Signal<void()>* drawSignal = NULL;
      ci::CameraUi camUi;
  };
}
