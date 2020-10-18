#include "cinder/app/App.h"

#include "WinSizeListener.h"

using namespace cms::cfg::components::utils;

WinSizeListener::WinSizeListener() {
  this->connections.push_back(
    ci::app::getWindow()->getSignalResize().connect(
      [this](){
        if (winSizeCallback) {
          winSizeCallback(ci::app::getWindowSize());
        }
      }
    ));
}

WinSizeListener::~WinSizeListener() {
  for(auto conn : connections) { conn.disconnect(); }
  connections.clear();
}

void WinSizeListener::setWinSizeCallback(std::function<void(const ci::ivec2&)> func) {
  winSizeCallback = func;
  winSizeCallback(ci::app::getWindowSize()); 
}

ci::ivec2 WinSizeListener::getWindowSize() const { return ci::app::getWindowSize(); }