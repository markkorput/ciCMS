#include "components.h"

void cms::cfg::components::addAllTo(cms::cfg::ctree::TreeBuilder& builder) {
  // configure our builder and configurator
  // builder.addDefaultInstantiator<Runner>("Runner");
  builder.addCfgObjectInstantiator<cms::cfg::components::Runner>("Runner");
  // builder.addDefaultInstantiator<Keyboard>("Keyboard");
  builder.addCfgObjectInstantiator<cms::cfg::components::Keyboard>("Keyboard");
  builder.addCfgObjectInstantiator<cms::cfg::components::Camera>("Camera");
}
