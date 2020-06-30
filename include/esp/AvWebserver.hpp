#ifndef AV_WEBSERVER_H
#define AV_WEBSERVER_H

#include <HTTPSServer.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <SSLCert.hpp>
#include "AvBusWriter.hpp"

using namespace httpsserver;

class HttpHeader{
  public:
    static constexpr const char* CONTENT_TYPE = "Content-Type"; 
};

class ContentType {
  public:
    static constexpr const char* APPLICATION_JSON = "application/json"; 
};

class AvWebserver {
 public:
 
  static void setup(AvBusWriter* writer);
  static void run();
  static void tearDown();

 private:
  static AvWebserver* instance;
  static bool isRunning;
  static void onNotFound(HTTPRequest* request, HTTPResponse * response);
  static void onGetRoot(HTTPRequest* request, HTTPResponse* response);
  static void onPostCommand(HTTPRequest* request, HTTPResponse* response);

  AvWebserver(AvBusWriter* writer);
  ~AvWebserver();

  void loop();

  SSLCert* certificate;
  HTTPSServer* server;
  AvBusWriter* writer;
};

#endif  // AV_WEBSERVER_H