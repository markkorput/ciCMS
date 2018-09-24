#pragma once

// cinder
#include "cinder/app/App.h"
// blocks
#include "ctree/signal.hpp"
#include "ciCMS/State.h"

class Keyboard {
  public:
    ~Keyboard() {
      for(auto conn : connections) {
        conn.disconnect();
      }

      connections.clear();
    }

  public:
    void onKeyDown(const std::string& key, std::function<void()> func) {
      if(key.size() != 1) return;
      char chr = key[0];

      this->connections.push_back(ci::app::getWindow()->getSignalKeyDown().connect([chr, func](ci::app::KeyEvent& event){
        if(event.getChar() == chr){
          func();
        }
      }));
    }

  private:
    std::vector<ci::signals::Connection> connections;
};
