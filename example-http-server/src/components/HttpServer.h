#pragma once

// cpp-netlib
#include <boost/network/protocol/http/server.hpp>
// ciCMS
#include "ctree/signal.hpp"

namespace http = boost::network::http;

class HttpServer {

  public: // private (sub-types)

    struct LambdaHandler;
    typedef http::server<LambdaHandler> LambdaHandlerServer;
    typedef std::function<void(LambdaHandlerServer::request const &request, LambdaHandlerServer::connection_ptr connection)> RequestHandlerFunc;

    using Request = LambdaHandlerServer::request;
    using ConnectionPtr = LambdaHandlerServer::connection_ptr;
    typedef std::function<bool(Request const &request, ConnectionPtr connection)> HttpHandlerFunc;

    class LambdaHandler {
      public:
        void operator()(LambdaHandlerServer::request const &request, LambdaHandlerServer::connection_ptr connection);

      private:
        void fallback(LambdaHandlerServer::request const &request, LambdaHandlerServer::connection_ptr connection);

      public:
        std::vector<HttpHandlerFunc> httpHandlerFuncs;
    };

  public: // API

    HttpServer();
    ~HttpServer() { this->stop(); }

    void update();

    bool start(bool forceRestart=false);
    void stop();

    void setPort(int port) { this->port = port; }
    void setQueue(bool queue) { this->bQueue = queue; }

  public: // signals

    // todo; put this signal in the our Handler class and give it a result collector that interrupts emits when a listener returns true
    ctree::Signal<void(const LambdaHandlerServer::request&, LambdaHandlerServer::connection_ptr)> requestSignal;

  private: // methods

    void serverThreadFunc();

    void process(LambdaHandlerServer::request const &request, LambdaHandlerServer::connection_ptr connection) {
      this->requestSignal.emit(request, connection);
    }

  private: // attributes

    std::string acceptAddress = "0.0.0.0";
    int port = 80;
    bool bQueue = false;

    LambdaHandler handler;
    std::shared_ptr<LambdaHandlerServer> serverRef = nullptr;
    bool bStarted = false;
    std::thread *thread = NULL;


    std::vector<std::pair<const LambdaHandlerServer::request&, LambdaHandlerServer::connection_ptr>> queue;
};
