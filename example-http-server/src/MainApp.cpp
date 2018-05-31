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
#include "ciCMS/Model.h"

// local
#include "Cfgr.h"
#include "Runner.h"
#include "Keyboard.h"

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
    void loadCfgData();

  private:
    cms::cfg::ctree::Builder<Cfgr> builder;
    Runner* pRunner;
};

void MainApp::setup(){
  // configure our builder and configurator
  builder.addDefaultInstantiator<Runner>("Runner");
  builder.addDefaultInstantiator<Keyboard>("Keyboard");
  this->loadCfgData();
  builder.getConfigurator()->cfg(*builder.getConfigurator(), "Cfgr");

  // build our application hierarchy
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
    case 'l': { loadCfgData(); break; }
    case 's': { }
  }
}

void MainApp::loadCfgData() {
  cms::ModelCollection tmpCol;

  // default; embedded config
  if(tmpCol.loadJsonFromFile(ci::app::getResourcePath("config.json")))
    CI_LOG_I("Loaded Resource config");

  // if found; AppData file settings will overwrite default resource file settings
  // std::string appdataPath = std::getenv("HOME");
  // appdataPath += "/Library/Application Support/APPLICATION_NAME/config.json";
  // if(tmpCol.loadJsonFromFile(appdataPath))
  //   CI_LOG_I("Loaded AppData config");

  // if found; config asset file settings will overwrite default resource file settings
  if(tmpCol.loadJsonFromFile(ci::app::getAssetPath("config.json")))
    CI_LOG_I("Loaded Asset config");

  builder.getModelCollection().loadJson(tmpCol.toJsonString());
}


#include <boost/network/protocol/http/server.hpp>

namespace http = boost::network::http;

/*<< Defines the server. >>*/
struct hello_world;
typedef http::server<hello_world> server;

/*<< Defines the request handler.  It's a class that defines two
     functions, `operator()` and `log()` >>*/
struct hello_world {
  /*<< This is the function that handles the incoming request. >>*/
  void operator()(server::request const &request, server::connection_ptr connection) {
    server::string_type ip = source(request);
    unsigned int port = request.source_port;
    std::ostringstream data;
    data << "Hello, " << ip << ':' << port << '!';

    std::map<std::string, std::string> headers = {
      {"Content-Length", "0"},
      {"Content-Type", "text/plain"},
    };

    auto body = data.str();
    headers["Content-Length"] = std::to_string(body.size());

    connection->set_status(server::connection::ok);
    connection->set_headers(headers);
    connection->write(body);
  }
};

int httpTest() {

  try {
    /*<< Creates the request handler. >>*/
    hello_world handler;
    /*<< Creates the server. >>*/
    server::options options(handler);
    server server_(options.address("0.0.0.0").port("8080")); //options.address(argv[1]).port(argv[2]));
    /*<< Runs the server. >>*/
    server_.run();
  }
  catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

CINDER_APP( MainApp, RendererGl )
