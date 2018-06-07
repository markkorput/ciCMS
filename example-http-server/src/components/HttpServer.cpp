#include "HttpServer.h"

#include "cinder/Log.h"

void HttpServer::LambdaHandler::operator()(LambdaHandlerServer::request const &request, LambdaHandlerServer::connection_ptr connection) {
  for(auto func : this->httpHandlerFuncs) {
    if (func(request, connection)) {
      return;
    }
  }

  this->fallback(request, connection);
}

void HttpServer::LambdaHandler::fallback(LambdaHandlerServer::request const &request, LambdaHandlerServer::connection_ptr connection) {
  // get client info
  HttpServer::LambdaHandlerServer::string_type ip = source(request);
  unsigned int port = request.source_port;

  // response body
  std::ostringstream data;
  data << "Hello, " << ip << ':' << port << '!';

  // response headers
  std::map<std::string, std::string> headers = {
    {"Content-Length", "0"},
    {"Content-Type", "text/plain"},
  };

  auto body = data.str();
  headers["Content-Length"] = std::to_string(body.size());

  // respond
  connection->set_status(HttpServer::LambdaHandlerServer::connection::ok);
  connection->set_headers(headers);
  connection->write(body);

  std::cout << "Responded to HTTP request with: " << body << std::endl;
}

HttpServer::HttpServer() {
  // Register http request handler
  this->handler.httpHandlerFuncs.push_back([this](LambdaHandlerServer::request const &request, LambdaHandlerServer::connection_ptr connection) {

    // add to process queue or process immediately, depending if queueing is enabled
    if (this->bQueue)
      this->queue.push_back(std::make_pair(request, connection));
    else
      this->process(request, connection);

    return true;
  });
}

void HttpServer::update() {
  for (auto &item : this->queue) {
    this->process(std::get<0>(item), std::get<1>(item));
  }
}

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
  // stop server
  if (serverRef) {
    this->serverRef->stop();
    this->serverRef = nullptr;
  }

  // wait until server thread finishes
  if (this->thread) {
    this->thread->join();
    this->thread = NULL;
  }

  bStarted = false;
}

void HttpServer::serverThreadFunc() {
  if(this->serverRef) this->serverRef->run();
}
