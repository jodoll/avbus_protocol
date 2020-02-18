#ifndef AV_WEBSERVER_H
#define AV_WEBSERVER_H

#include <RichHttpServer.h>
#include <WiFi.h>
#include "AvBusWriter.hpp"

using RichHttpConfig = RichHttp::Generics::Configs::EspressifBuiltin;
using RequestContext = RichHttpConfig::RequestContextType;

class AvWebserver {
 public:
  AvWebserver(AvBusWriter* writer);
  ~AvWebserver();

  void start();
  void handleConnections();

 private:
  SimpleAuthProvider authProvider;
  RichHttpServer<RichHttpConfig> server;
  AvBusWriter* writer;

  void onGetRoot(RequestContext request);
  void onPostCommand(RequestContext request);
};

#endif  // AV_WEBSERVER_H