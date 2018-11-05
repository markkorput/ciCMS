#include "Runner.h"

using namespace cms::cfg::components;

bool Runner::update() {
  if (bDone) return false;
  this->updateSignal.emit();
  return true;
}

void Runner::draw() {
  this->drawSignal.emit();
}

void Runner::cfg(cms::cfg::Cfg& cfg){
  cfg
  .connectAttr<void()>("exitOn", [this](){ this->bDone = true; })

  // .pushRef("drawState", this->bDrawEnabled)
  .withSignalByAttr<void()>("drawEmit", [this](::ctree::Signal<void()>& sig){
    this->drawSignal.connect([&sig](){ sig.emit(); });
  });
}
