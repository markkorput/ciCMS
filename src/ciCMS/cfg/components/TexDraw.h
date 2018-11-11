#pragma once

#include "cinder/gl/gl.h"
#include "ciCMS/cfg/Cfg.h"

namespace cms { namespace cfg { namespace components {

  class TexDraw {
    public:
      void cfg(cms::cfg::Cfg& cfg);

    private:
      void draw();

    private: //attrs

      bool verbose = false;
      // std::vector<ci::signals::Connection> connections;
      ci::gl::TextureRef mTex = nullptr;
      std::shared_ptr<ci::Rectf> rectRef = nullptr;
      std::shared_ptr<ci::Area> fitCenteredAreaRef = nullptr;
      std::shared_ptr<ci::Area> fillCenteredAreaRef = nullptr;
  };
}}}
