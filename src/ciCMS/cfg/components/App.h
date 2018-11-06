#pragma once

// cinder
#include "cinder/app/App.h"
// blocks
#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/components/Runner.h"

namespace cms { namespace cfg { namespace components {
  class App : public ci::app::App {
    public:
      // MainApp();
      void setup() override;
      void cleanup() override;
      void update() override;
      void draw() override;
      void keyDown(ci::app::KeyEvent event) override;

    protected:
      cms::cfg::ctree::TreeBuilder builder;
      cms::cfg::components::Runner* pRunner;
  };
}}}
