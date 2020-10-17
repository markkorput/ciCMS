#include "Script.h"

using namespace cms::cfg::components;


void Script::cfg(cms::cfg::Cfg& cfg){
  Base::cfg(cfg);

  cfg
  .with("do", [this, &cfg](const std::string& script){
    this->func = cfg.compileScript(script);
  })
  .reader();
}

void Script::run() {
  verbose("Script::run");
  if (func) func();
  else verbose("Script::run NO FUNC");
}
