#include "Camera.h"
#include "cinder/gl/gl.h"

using namespace cms::cfg::components;

void Camera::cfg(cms::cfg::Cfg& cfg) {
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

void Camera::render() {
  // TODO; push matrix?
  if (drawSignal) {
    ci::gl::setMatrices(cam);
    drawSignal->emit();
  }
}
