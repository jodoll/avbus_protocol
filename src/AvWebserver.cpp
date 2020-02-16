#include "AvWebserver.hpp"

AvWebserver::AvWebserver() : server(80, authProvider) {}
AvWebserver::~AvWebserver() {}

void AvWebserver::start() {
  server
      .buildHandler("/")  //
      .on(HTTP_GET, [this](RequestContext request) { onGetRoot(request); });
  server
      .buildHandler("/api/v1/remote/command")  //
      .on(HTTP_POST, [this](RequestContext request) { onPostCommand(request); });

  server.clearBuilders();
  server.begin();
}

void AvWebserver::handleConnections() { server.handleClient(); }

void AvWebserver::onGetRoot(RequestContext request) {
  const char* buffer = "Hello World";

  request.response.setCode(200);
  request.response.json["message"] = buffer;
}

void AvWebserver::onPostCommand(RequestContext request) {
  JsonObject body = request.getJsonBody().as<JsonObject>();

  if (body.isNull()) {
    request.response.setCode(400);
    request.response.json["error"] = F("Invalid JSON.  Must be an object.");
    return;
  }

  const char* KEY_DEVICE = "device";
  if (!body.containsKey(KEY_DEVICE) || !body[KEY_DEVICE].is<std::string>()) {
    request.response.setCode(400);
    request.response.json["error"] = F("Invalid JSON.  Missing or invalid value for 'device'");
    return;
  }
  std::string device = body[KEY_DEVICE];

  const char* KEY_COMMAND = "command";
  if (!body.containsKey(KEY_COMMAND) || !body[KEY_COMMAND].is<std::string>()) {
    request.response.setCode(400);
    request.response.json["error"] = F("Invalid JSON.  Missing or invalid value for 'command'");
    return;
  }
  std::string command = body[KEY_COMMAND];

  Serial.print("Device: ");
  Serial.println(device.c_str());
  Serial.print("Command: ");
  Serial.println(command.c_str());

  request.response.setCode(200);
  request.response.json["success"] = true;
}