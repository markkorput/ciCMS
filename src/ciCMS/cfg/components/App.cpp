#include "App.h"

using namespace cms::cfg::components;

bool App::update() {
  if (bDone) return false;
  this->updateSignal.emit();
  return true;
}

void App::draw() {
  this->drawSignal.emit();
}

void App::cfg(cms::cfg::Cfg& cfg){
  cfg
  .connectAttr<void()>("exitOn", [this](){ this->bDone = true; })

  // .pushRef("drawState", this->bDrawEnabled)
  .withSignalByAttr<void()>("drawEmit", [this](::ctree::Signal<void()>& sig){
    this->drawSignal.connect([&sig](){ sig.emit(); });
  });
}
