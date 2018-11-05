#include "Keyboard.h"

using namespace cms::cfg::components;

Keyboard::~Keyboard() {
  for(auto conn : connections) {
    conn.disconnect();
  }

  connections.clear();
}

void Keyboard::onKeyDown(const std::string& key, std::function<void()> func) {
  if(key.size() != 1) return;
  char chr = key[0];

  this->connections.push_back(ci::app::getWindow()->getSignalKeyDown().connect([chr, func](ci::app::KeyEvent& event){
    if(event.getChar() == chr){
      func();
    }
  }));
}

void Keyboard::cfg(cms::cfg::Cfg& cfg){
  cfg.reader()
  ->withRegex("^key:(.)$", [this, &cfg](const std::smatch& match, const std::string& v){
    // compile lambda func based on script in attribute value
    auto func = cfg.compileScript(v);
    // register func as action to be performed when the specified key is pressed
    this->onKeyDown(match[1], func);
  });
}
