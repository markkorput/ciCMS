#pragma once

#include "ctree/signal.hpp"
#include "ciCMS/cfg/Cfg.h"

namespace cms { namespace cfg { namespace components {
  class App {
    public: // methods

      inline bool update() {
        if (bDone) return false;
        this->updateSignal.emit();
        return true;
      }

      inline void draw() {
        this->drawSignal.emit();
      }

      inline void fileDrop( const cinder::app::FileDropEvent& event ) {
          this->fileDropSignal.emit(event);
      }

      void resize();

      void cfg(cms::cfg::Cfg& cfg);

    private: // attributes
      bool bDone = false;
      bool verbose = false;

      ::ctree::Signal<void()> updateSignal;
      ::ctree::Signal<void()> drawSignal;
      ::ctree::Signal<void(const ci::Area&)> windowBoundsSignal;
      ::ctree::Signal<void(const cinder::app::FileDropEvent&)> fileDropSignal;
  };
}}}
