#pragma once

#include "ctree/signal.hpp"
#include "ciCMS/State.h"

class Runner {
  public:
    void draw() {
      if(drawState.val())
        drawSignal.emit();
    }

  public:

    void cfg(cms::cfg::Cfg& cfg){
      cfg.getSignal<void()>("signal1")->emit();

      cfg.withSignalByAttr<void()>("drawEmit", [this](::ctree::Signal<void()>& sig){
        this->drawSignal.connect([&sig](){ sig.emit(); });
      });

      cfg.withStateByAttr<bool>("drawState", [this](cms::State<bool>& state) {
        state = true;
        state.push(this->drawState);
      });

      // cfg
      // .connect("drawEmit", )
      // ->with("drawEmit", [this, &obj](const std::string& v){
      //   auto pSignal = this->getSignal<void()>(v);
      //   obj.drawSignal.connect([pSignal](){ pSignal->emit(); });
      // })
      // .with("drawState", [this, &obj](const std::string& v){
      //   auto pState = this->getState<bool>(v);
      //   pState->operator=(true);
      //   // obj.drawSignal.connect([pSignal](){ pSignal->emit(); });
      //   pState->push(obj.drawState);
      // });
    }

  public:
    ctree::Signal<void()> drawSignal;
    cms::State<bool> drawState;
};
