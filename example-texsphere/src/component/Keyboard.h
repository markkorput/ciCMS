#pragma once

#include "ctree/signal.hpp"
#include "ciCMS/cfg/Cfg.h"

namespace component {

  class Keyboard {
    public:
      ~Keyboard();
      void cfg(cms::cfg::Cfg& cfg);

    private:
      void onKeyDown(const std::string& key, std::function<void()> func);

    private:
      std::vector<ci::signals::Connection> connections;
  };

}
