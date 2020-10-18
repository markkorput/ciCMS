#pragma once

#include "cinder/gl/gl.h"
#include "ciCMS/cfg/Cfg.h"
#include "Base.h"
#include "utils/WinSizeListener.h"

namespace cms { namespace cfg { namespace components {

   class TexDraw : public Base {
    public:
      void cfg(cms::cfg::Cfg& cfg);

    private:
      void draw();
      void updateWindowSize(const ci::ivec2& size);

    private: //attrs
      bool bFillViewport = false;
      // std::vector<ci::signals::Connection> connections;
      ci::gl::TextureRef mTex = nullptr;
      std::shared_ptr<ci::Rectf> rectRef = nullptr;
      std::shared_ptr<ci::vec4> winPercentagebounds = nullptr;

      std::shared_ptr<ci::Area> fitCenteredAreaRef = nullptr;
      std::shared_ptr<ci::Area> fillCenteredAreaRef = nullptr;

      utils::WinSizeListenerRef winSizeListenerRef;
  };
}}}
