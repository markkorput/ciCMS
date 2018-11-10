#include <iostream>
#include "ciCMS/cfg/components/components.h"
#include "components/VrInterface.h"

// All our custom MainApp class really does is register the components
// that can be instantiated. After that, it tries to load the config.json
// asset file and to build "App" root component.

class MainApp : public cms::cfg::components::CinderApp {
  public:
    MainApp();
};

MainApp::MainApp() {
  configureBuilder([](cms::cfg::ctree::TreeBuilder& builder){
    // finally configure the builder using the "Builder" data
    builder.cfg("Builder");

    // register our application's custom components (CinderApp will by default register all standard)
    // components in its setup method
    builder.addCfgObjectInstantiator<components::VrInterface>("VrInterface");
  });
}


//#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"

// CINDER_APP( cms::cfg::components::CinderApp, ci::app::RendererGl )
CINDER_APP( MainApp, ci::app::RendererGl )
