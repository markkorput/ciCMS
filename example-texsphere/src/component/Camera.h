#pragma once

#include "cinder/app/App.h"
#include "ctree/signal.hpp"
#include "cinder/gl/gl.h"
#include "cinder/Signals.h"
#include "cinder/CameraUi.h"

namespace component {
  class Camera {

    public:

      void cfg(cms::cfg::Cfg& cfg){
        cfg
        .connectAttr<void()>("on", [this](){ this->render(); })
        .withSignalByAttr<void()>("emit", [this](ci::signals::Signal<void()>& sig) {
          this->drawSignal = &sig;
        })
        .reader()
          ->withVec3("pos", [this](const glm::vec3& val){ this->cam.setEyePoint(val); })
          .withVec3("lookAt", [this](const glm::vec3& val){ this->cam.lookAt(val); });

        if (cfg.reader()->getBool("ui", false)) {
          camUi = ci::CameraUi( &cam, ci::app::getWindow() );
          camUi.connect(ci::app::getWindow() );
          camUi.enable();
        } else {
          camUi.disable();
        }
      }

    private:

      void render() {
        // TODO; push matrix?
        ci::gl::setMatrices(cam);
        if (drawSignal) drawSignal->emit();
      }

    private:
      ci::CameraPersp cam;
      ci::signals::Signal<void()>* drawSignal = NULL;
      ci::CameraUi camUi;
  };
}
