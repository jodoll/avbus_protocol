#ifndef AV_WEBSERVER_H
#define AV_WEBSERVER_H

#include <RichHttpServer.h>
#include <WiFi.h>

using RichHttpConfig = RichHttp::Generics::Configs::EspressifBuiltin;
using RequestContext = RichHttpConfig::RequestContextType;

class AvWebserver {
 public:
  AvWebserver();
  ~AvWebserver();

  void start();
  void handleConnections();

 private:
  SimpleAuthProvider authProvider;
  RichHttpServer<RichHttpConfig> server;

  void onGetRoot(RequestContext request);
  void onPostCommand(RequestContext request);
};

#endif  // AV_WEBSERVER_H