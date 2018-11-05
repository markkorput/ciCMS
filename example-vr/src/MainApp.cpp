// stdlib
#include <iostream>
// cinder
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
// blocks
#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/cfg/components.h"
#include "ciCMS/Model.h"
// local
#include "Cfgr.h"
// #include "component/Runner.h"

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
    component::Runner* pRunner;
};

void MainApp::setup(){
  cms::cfg::components::addAllTo(builder);

  builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("config.json"));
  builder.getConfigurator()->cfg(*builder.getConfigurator(), "Cfgr");

  // build our application hierarchy
  pRunner = builder.build<component::Runner>("Runner");
}

void MainApp::cleanup() {
  if (this->pRunner) {
      builder.destroy<component::Runner>(this->pRunner);
      this->pRunner = NULL;
  }
}

void MainApp::update(){
  if (!pRunner->update()) quit();
}

void MainApp::draw(){
  ci::gl::clear(Color(0,0,0));
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
