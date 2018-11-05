#pragma once

#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/Cfg.h"
#include "Runner.h"
#include "Keyboard.h"
#include "Camera.h"

namespace cms { namespace cfg { namespace components {
  void addAllTo(::cms::cfg::ctree::Builder<Cfgr>& builder);
}
