#include "App.h"
#include "cinder/app/App.h"

using namespace cms::cfg::components;
using namespace cinder;

void App::cfg(cms::cfg::Cfg& cfg){
  cfg
  .connectAttr<void()>("exitOn", [this](){ this->bDone = true; })

  // .pushRef("drawState", this->bDrawEnabled)
  .withSignalByAttr<void()>("drawEmit", [this](::ctree::Signal<void()>& sig){
    this->drawSignal.connect([&sig](){ sig.emit(); });
  })

  .withSignalByAttr<void(const ci::app::FileDropEvent&)>("fileDropEmit",
    [this](::ctree::Signal<void(const ci::app::FileDropEvent&)>& sig){
      this->fileDropSignal.connect([&sig](const ci::app::FileDropEvent& evt){ sig.emit(evt); });
    })

  .withSignalByAttr<void(const cinder::fs::path&)>("fileDropPathEmit",
    [this](::ctree::Signal<void(const cinder::fs::path&)>& sig){
      this->fileDropSignal.connect([&sig](const ci::app::FileDropEvent& evt){

        if (evt.getNumFiles() == 1) {
          auto path = evt.getFile(0);
          sig.emit(path);
        }
      });
    })

  .withSignalByAttr<void(const Area&)>("windowResizeBoundsEmit",
    [this](::ctree::Signal<void(const Area&)>& sig){
      this->windowBoundsSignal.connect([&sig](const Area& boundsArea) {
        auto tmp = boundsArea;
        sig.emit(tmp);
      });
    });
}

void App::resize() {
  auto area = app::getWindowBounds () ;
  this->windowBoundsSignal.emit(area);
}
