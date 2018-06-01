#include "HttpServer.h"

#include "cinder/Log.h"

// HttpServer::RequestHandler::operator()(HttpServer::LambdaHandlerServer::request const &request, HttpServer::LambdaHandlerServer::connection_ptr connection) {
//   // get client info
//   HttpServer::LambdaHandlerServer::string_type ip = source(request);
//   unsigned int port = request.source_port;
//
//   // response body
//   std::ostringstream data;
//   data << "Hello, " << ip << ':' << port << '!';
//
//   // response headers
//   std::map<std::string, std::string> headers = {
//     {"Content-Length", "0"},
//     {"Content-Type", "text/plain"},
//   };
//
//   auto body = data.str();
//   headers["Content-Length"] = std::to_string(body.size());
//
//   // respond
//   connection->set_status(HttpServer::LambdaHandlerServer::connection::ok);
//   connection->set_headers(headers);
//   connection->write(body);
// }

// void HttpServer::operator()(HttpServer::LambdaHandlerServer::request const &request, HttpServer::LambdaHandlerServer::connection_ptr connection) {
//   // get client info
//   HttpServer::LambdaHandlerServer::string_type ip = source(request);
//   unsigned int port = request.source_port;
//
//   // response body
//   std::ostringstream data;
//   data << "Hello, " << ip << ':' << port << '!';
//
//   // response headers
//   std::map<std::string, std::string> headers = {
//     {"Content-Length", "0"},
//     {"Content-Type", "text/plain"},
//   };
//
//   auto body = data.str();
//   headers["Content-Length"] = std::to_string(body.size());
//
//   // respond
//   connection->set_status(HttpServer::LambdaHandlerServer::connection::ok);
//   connection->set_headers(headers);
//   connection->write(body);
// }

bool HttpServer::start(bool forceRestart) {
  if (bStarted) {
    if (!forceRestart) return false;
    this->stop();
  }

  bool success = false;

  try {
    // Creates the server.
    CI_LOG_I("Initializing HttpServer to listen to: " << this->acceptAddress << ":" << this->port);
    LambdaHandlerServer::options options(handler);
    this->serverRef = std::make_shared<LambdaHandlerServer>(
      options
      .address(this->acceptAddress)
      .port(std::to_string(this->port))); //options.address(argv[1]).port(argv[2]));

    // Runs the server.
    CI_LOG_I("Starting HttpServer thread");
    this->thread = new std::thread(std::bind(&HttpServer::serverThreadFunc, this));
    success = true;
  }
  catch (std::exception &e) {
    std::cerr << "Exception while starting HttpServer: " << std::endl << e.what() << std::endl << std::endl;
    success = false;
  }

  bStarted = success;
  return success;
}

void HttpServer::stop() {
  // TODO
  if (serverRef) {
    this->serverRef->stop();
    this->serverRef = nullptr;
  }

  // serverRef = nullptr;
  if (this->thread) {
    this->thread->join();
    this->thread = NULL;
  }

  bStarted = false;
}

void HttpServer::serverThreadFunc() {
  if(this->serverRef) this->serverRef->run();
}
