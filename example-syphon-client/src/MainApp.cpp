// cinder
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
// blocks
#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/Model.h"
#include "cinderSyphon.h"
// local
#include "Cfgr.h"
#include "Runner.h"
#include "SyphonClientRenderer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MainApp : public App {
  public:
    // MainApp();
    void setup() override;
    void update() override;
    void draw() override;
    void keyDown(KeyEvent event) override;

  // private:
    cms::cfg::ctree::Builder<Cfgr> builder;
    Runner* pRunner;
};

void MainApp::setup(){
  builder.addDefaultInstantiator<Runner>("Runner");
  builder.addDefaultInstantiator<syphonClient>("syphonClient");
  builder.addDefaultInstantiator<Runner>("SyphonClientRenderer");
  builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("config.json"));

  pRunner = builder.build<Runner>("Runner");
  CI_LOG_I("pRunner: " << pRunner);
}

void MainApp::update(){
}

void MainApp::draw(){
  gl::clear(Color(0,0,0));
}

void MainApp::keyDown(KeyEvent event){
  // switch(event.getChar()){
  //   case 'l': {
  //   }
  //   case 's': {
  //   }
  // }
}

CINDER_APP( MainApp, RendererGl )
