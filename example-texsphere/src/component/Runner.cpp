#include "Runner.h"

using namespace component;

void Runner::draw() {
  if(this->bDrawEnabled) {
    this->drawSignal.emit();
  }
}

void Runner::cfg(cms::cfg::Cfg& cfg){
  cfg
  .pushRef("drawState", this->bDrawEnabled)
  .withSignalByAttr<void()>("drawEmit", [this](::ctree::Signal<void()>& sig){
    this->drawSignal.connect([&sig](){ sig.emit(); });
  });
}
