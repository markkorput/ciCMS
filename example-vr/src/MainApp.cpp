
#include "ciCMS/cfg/components/components.h"
#include "components/VrInterface.h"

// All our custom MainApp class really does is register the components
// that can be instantiated. After that, it tries to load the config.json
// asset file and to build "Runner" root component.

class MainApp : public cms::cfg::components::App {
  public:
    MainApp();
};

MainApp::MainApp() {
  configureBuilder([](cms::cfg::ctree::TreeBuilder& builder){
    // TODO; add components to builder here
    builder.addCfgObjectInstantiator<components::VrInterface>("VrInterface");
  });
}


//#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"

// CINDER_APP( cms::cfg::components::App, ci::app::RendererGl )
CINDER_APP( MainApp, ci::app::RendererGl )
