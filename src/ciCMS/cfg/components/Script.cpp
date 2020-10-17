#include "Script.h"

using namespace cms::cfg::components;


void Script::cfg(cms::cfg::Cfg& cfg){
  Base::cfg(cfg);

  cfg
  .connectAttr<void()>("on", [this](){
    this->verbose("Script::run");
    if (this->func) this->func();
    else this->verbose("Script::run NO FUNC");  
  })
  .with("do", [this, &cfg](const std::string& script){
    this->func = cfg.compileScript(script);
  })
  .reader();
}

