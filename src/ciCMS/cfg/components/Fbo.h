#pragma once

#include "ciCMS/cfg/Cfg.h"
#include "cinder/gl/gl.h"

namespace cms { namespace cfg { namespace components {

  class Fbo {

    public:
      void cfg(cms::cfg::Cfg& cfg);

    private: // methods
      void init(const ci::ivec2& size);
      void draw();

    private:

      bool verbose =false;
      bool bClear = false;
      bool bSetMatricesWindow = false;
      bool bSetViewport = true;

      ci::ColorA clearColor = ci::ColorA(0,0,0,0);
      ci::gl::FboRef fboRef = nullptr;

      ::ctree::Signal<void()>* drawSignal = NULL;
      ::ctree::Signal<void(const ci::Area&)>* boundsSignal = NULL;
      ::ctree::Signal<void(ci::gl::TextureRef)>* textureSignal = NULL;
  };

}}}
