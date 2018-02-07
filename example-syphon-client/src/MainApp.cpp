// stdlib
#include <iostream>
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
    void cleanup() override;
    void update() override;
    void draw() override;
    void keyDown(KeyEvent event) override;

  private:
    cms::cfg::ctree::Builder<Cfgr> builder;
    Runner* pRunner;
};

void MainApp::setup(){
  builder.addDefaultInstantiator<Runner>("Runner");
  builder.addDefaultInstantiator<syphonClient>("SyphonClient");
  builder.addDefaultInstantiator<SyphonClientRenderer>("SyphonClientRenderer");
  builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("config.json"));

  builder.getConfigurator()->cfg(*builder.getConfigurator(), "Cfgr");
  pRunner = builder.build<Runner>("Runner");
}

void MainApp::cleanup() {
  if (this->pRunner) {
      builder.destroy<Runner>(this->pRunner);
      this->pRunner = NULL;
  }
}

void MainApp::update(){
}

void MainApp::draw(){
  gl::clear(Color(0,0,0));
  pRunner->draw();
}

void MainApp::keyDown(KeyEvent event){
  switch(event.getChar()){
    case 'l': {
      std::cout << "Re-Loading json data" << std::endl;
      builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("config.json"));
      std::cout << "JSON collection size: " << builder.getModelCollection().size() << std::endl;
    }
    case 's': {
    }
  }
}

CINDER_APP( MainApp, RendererGl )
