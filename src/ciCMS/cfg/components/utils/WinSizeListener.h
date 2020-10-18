#pragma once

#include "cinder/gl/gl.h"
#include "ciCMS/cfg/Cfg.h"

namespace cms { namespace cfg { namespace components { namespace utils {

  class WinSizeListener;
  typedef std::shared_ptr<WinSizeListener> WinSizeListenerRef;

  class WinSizeListener {

    public:
      WinSizeListener();
      ~WinSizeListener();

      void setWinSizeCallback(std::function<void(const ci::ivec2&)> func);
      ci::ivec2 getWindowSize() const;

    private:
      std::vector<ci::signals::Connection> connections;
      std::function<void(const ci::ivec2&)> winSizeCallback = nullptr;
  };
}}}}