#pragma once

// cinder
#include "cinder/app/App.h"
// blocks
#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/components/App.h"

namespace cms { namespace cfg { namespace components {
  class CinderApp : public ci::app::App {
    public:
      // MainApp();
      void setup() override;
      void cleanup() override;
      void update() override;
      void draw() override;
      void keyDown(ci::app::KeyEvent event) override;
      void fileDrop(ci::app::FileDropEvent event) override;

    protected:
      void configureBuilder(std::function<void(cms::cfg::ctree::TreeBuilder&)> func);

    private:
      cms::cfg::ctree::TreeBuilder builder;
    protected:
      cms::cfg::components::App* pAppComponent;
  };
}}}
