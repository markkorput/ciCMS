// cinder
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
// ciCMS
#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/components/components.h"

using namespace std;
using namespace cms::cfg::components;

void CinderApp::setup(){
  // Add ciCMS's standard components to our builder
  cms::cfg::components::addAllTo(builder);

  // populate our builder with our config.json data
  builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("config.json"));

  // build "App" as our application's hierarchy root component
  pAppComponent = builder.build<App>("App");

  if (pAppComponent == NULL) {
    std::cerr << "Could not load \"App\" root component" << std::endl;
    quit();
  }
}

void CinderApp::cleanup() {
  if (this->pAppComponent) {
      builder.destroy<App>(this->pAppComponent);
      this->pAppComponent = NULL;
  }
}

void CinderApp::update(){
  if (pAppComponent && !pAppComponent->update()) quit();
}

void CinderApp::draw(){
  // ci::gl::clear(ci::Color(0,0,0));
  if (pAppComponent) pAppComponent->draw();
}

void CinderApp::resize() {
  if (this->pAppComponent) { this->pAppComponent->resize(); }
}

void CinderApp::keyDown(ci::app::KeyEvent event){
  switch(event.getChar()){
    case 'l': {
      std::cout << "Re-Loading json data" << std::endl;
      builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("config.json"));
      std::cout << "JSON collection size: " << builder.getModelCollection().size() << std::endl;
    }
    case 's': {
    }
    case 'r': {
      if (event.isControlDown()) {
        builder.reset();
        builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("config.json"));

        CI_LOG_I("RELOADING ROOT APP COMPONENT");
        // build "App" as our application's hierarchy root component
        pAppComponent = builder.build<App>("App");

        if (pAppComponent == NULL) {
          CI_LOG_E("Could not load \"App\" root component");
          quit();
        }
      }
    }
  }
}

void CinderApp::fileDrop( ci::app::FileDropEvent event ) {
  if (this->pAppComponent) this->pAppComponent->fileDrop(event);
}

void CinderApp::configureBuilder(std::function<void(cms::cfg::ctree::TreeBuilder&)> func) {
  func(this->builder);
}
