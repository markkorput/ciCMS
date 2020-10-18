// cinder
#include "cinder/gl/gl.h"
// blocks
#include "ciCMS/State.h"
#include "Runner.h"

using namespace cms::cfg::components;

void Runner::setup() {
  setupSignal.emit();
}

void Runner::exit() {
  exitSignal.emit();
}

void Runner::draw() {
  verbose("Runner::draw");
  // ci::gl::ScopedMatrices matScope;
  ci::gl::pushMatrices();
  ci::gl::translate(mTranslate);
  ci::gl::scale(mScale);
  drawSignal.emit();
  ci::gl::popMatrices();
}

void Runner::update() {
  updateSignal.emit();
}

void Runner::notifyAboutConfig() {
  configSignal.emit();
}

void Runner::cfg(cfg::Cfg& cfg) {
  cms::cfg::components::Base::cfg(cfg);

  cfg
  .setVec3("scale", this->mScale)
  .setVec3("translate", this->mTranslate)

  .connectAttr<void()>("drawOn", [this](){ this->draw(); })

  .connectAttr(this->setupSignal, "setupEmit")
  .connectAttr(this->exitSignal, "exitEmit")
  .connectAttr(this->drawSignal, "drawEmit")
  .connectAttr(this->updateSignal, "updateEmit")
  .connectAttr(this->configSignal, "cofigEmit")
  ;
}
