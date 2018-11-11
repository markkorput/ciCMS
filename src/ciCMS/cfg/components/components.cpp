#include "components.h"

void cms::cfg::components::addAllTo(cms::cfg::ctree::TreeBuilder& builder) {
  // configure our builder and configurator
  // builder.addDefaultInstantiator<Runner>("Runner");
  builder.addCfgObjectInstantiator<cms::cfg::components::Runner>("Runner"); // deprecated; replaced by App
  builder.addCfgObjectInstantiator<cms::cfg::components::App>("App");
  // builder.addDefaultInstantiator<Keyboard>("Keyboard");
  builder.addCfgObjectInstantiator<cms::cfg::components::Keyboard>("Keyboard");
  builder.addCfgObjectInstantiator<cms::cfg::components::Camera>("Camera");
  builder.addCfgObjectInstantiator<cms::cfg::components::Video>("Video");
  builder.addCfgObjectInstantiator<cms::cfg::components::TexDraw>("TexDraw");
  builder.addCfgObjectInstantiator<cms::cfg::components::Fbo>("Fbo");
}
