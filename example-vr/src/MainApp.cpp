// cinder
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
// ciCMS
#include "ciCMS/cfg/components/components.h"
// local
#include "components/VrInterface.h"

class MainApp : public cms::cfg::components::App {
  public:
    void setup() override;
};

void MainApp::setup() {
  cms::cfg::components::App::setup();

  // TODO; add components to builder here
  builder.addCfgObjectInstantiator<components::VrInterface>("VrInterface");
}

// CINDER_APP( cms::cfg::components::App, ci::app::RendererGl )
CINDER_APP( MainApp, ci::app::RendererGl )
