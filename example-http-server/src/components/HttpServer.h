#pragma once

#include <boost/network/protocol/http/server.hpp>

namespace http = boost::network::http;

class HttpServer {

  private: // private (sub-types)

    struct LambdaHandler;
    typedef http::server<LambdaHandler> LambdaHandlerServer;
    typedef std::function<void(LambdaHandlerServer::request const &request, LambdaHandlerServer::connection_ptr connection)> RequestHandlerFunc;

    struct LambdaHandler {
      void operator()(LambdaHandlerServer::request const &request, LambdaHandlerServer::connection_ptr connection) {
        // if (func) func(request, connection);

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
      };

      RequestHandlerFunc func = nullptr;
    };

  public:

    ~HttpServer() { this->stop(); }

    void setPort(int port) {
      this->port = port;
    }

    bool start(bool forceRestart=false);
    void stop();

  private:

    void serverThreadFunc();

  private: // attributes

    std::string acceptAddress = "0.0.0.0";
    int port = 80;

    LambdaHandler handler;
    std::shared_ptr<LambdaHandlerServer> serverRef = nullptr;
    bool bStarted = false;
    std::thread *thread = NULL;
};
