#include "App.h"
#include "cinder/app/App.h"

using namespace cms::cfg::components;
using namespace cinder;

void App::cfg(cms::cfg::Cfg& cfg){
  cfg
  .setBool("verbose", this->verbose)
  .connectAttr<void()>("exitOn", [this](){ this->bDone = true; })

  // .pushRef("drawState", this->bDrawEnabled)
  .withSignalByAttr<void()>("updateEmit", [this](::ctree::Signal<void()>& sig){
    this->updateSignal.connect([&sig](){ sig.emit(); });
  })

  // .pushRef("drawState", this->bDrawEnabled)
  .withSignalByAttr<void()>("drawEmit", [this](::ctree::Signal<void()>& sig){
    this->drawSignal.connect([&sig](){ sig.emit(); });
  })

  .withSignalByAttr<void(const ci::app::FileDropEvent&)>("fileDropEmit",
    [this](::ctree::Signal<void(const ci::app::FileDropEvent&)>& sig){
      if (verbose) CI_LOG_I("App fileDropEmit");
      this->fileDropSignal.connect([&sig](const ci::app::FileDropEvent& evt){ sig.emit(evt); });
    })

  .withSignalByAttr<void(const cinder::fs::path&)>("fileDropPathEmit",
    [this](::ctree::Signal<void(const cinder::fs::path&)>& sig){
      this->fileDropSignal.connect([this, &sig](const ci::app::FileDropEvent& evt){
        if (verbose) CI_LOG_I("App fileDropPathEmit");

        if (evt.getNumFiles() == 1) {
          auto path = evt.getFile(0);
          sig.emit(path);
        }
      });
    })

  .withSignalByAttr<void(const Area&)>("windowResizeBoundsEmit",
    [this](::ctree::Signal<void(const Area&)>& sig){
      this->windowBoundsSignal.connect([&sig](const Area& boundsArea) {
        sig.emit(boundsArea);
      });
    });
}

void App::resize() {
  if (verbose) CI_LOG_I("App resize");
  auto area = app::getWindowBounds () ;
  this->windowBoundsSignal.emit(area);
}
