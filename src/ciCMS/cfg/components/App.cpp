// cinder
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
// blocks
#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/components/components.h"

using namespace std;
using namespace cms::cfg::components;

void App::setup(){
  // Add ciCMS's standard components to our builder
  cms::cfg::components::addAllTo(builder);
  // populate our builder with our config.json data
  builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("config.json"));

  // build "Runner" as our application's hierarchy root node
  pRunner = builder.build<Runner>("Runner");
}

void App::cleanup() {
  if (this->pRunner) {
      builder.destroy<Runner>(this->pRunner);
      this->pRunner = NULL;
  }
}

void App::update(){
  if (!pRunner->update()) quit();
}

void App::draw(){
  ci::gl::clear(ci::Color(0,0,0));
  pRunner->draw();
}

void App::keyDown(ci::app::KeyEvent event){
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


void App::configureBuilder(std::function<void(cms::cfg::ctree::TreeBuilder&)> func) {
  func(this->builder);
}
