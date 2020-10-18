#pragma once

#include "ciCMS/cfg/Cfg.h"
#include "cinder/Log.h"

namespace cms { namespace cfg { namespace components {
  class Base {

    public:
      inline void cfg(cms::cfg::Cfg& cfg) {
        cfg
        .setBool("verbose", this->bVerbose)
        .setBool("enabled", this->bEnabled)
        .pushRef<bool>("enabledState", this->bEnabled)
        .set("name", this->name);
      }

      inline void verbose(const std::string& msg) const {
        if (bVerbose) CI_LOG_I("["+name+"] "+msg);
      }

      inline bool isEnabled() const { return bEnabled; }

    private:
      bool bVerbose = false;
      bool bEnabled = true;
      std::string name = "NoName";
  };
}}}
