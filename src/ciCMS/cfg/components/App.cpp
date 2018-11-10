#include "App.h"

using namespace cms::cfg::components;

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
    });
}
