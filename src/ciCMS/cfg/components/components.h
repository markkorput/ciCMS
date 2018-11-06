#pragma once

#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/Cfg.h"
#include "Runner.h"
#include "Keyboard.h"
#include "Camera.h"

#include "App.h"

namespace cms { namespace cfg { namespace components {
  void addAllTo(cms::cfg::ctree::TreeBuilder& builder);
}}}