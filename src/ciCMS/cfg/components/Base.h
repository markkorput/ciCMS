#pragma once

#include "ciCMS/cfg/Cfg.h"
#include "cinder/Log.h"

namespace cms { namespace cfg { namespace components {
  class Base {

    public:
      inline void cfg(cms::cfg::Cfg& cfg) {
        cfg.setBool("verbose", this->bVerbose);
      }

      inline void verbose(const std::string& msg) {
        if (bVerbose) CI_LOG_I(msg);
      }

    protected:
      bool bVerbose = false;
  };
}}}
