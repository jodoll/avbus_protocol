#include "esp/AvWebserver.hpp"
#include "esp/Command.hpp"
#include <ArduinoJson.hpp>

using namespace httpsserver;

AvWebserver* AvWebserver::instance = nullptr; 
bool AvWebserver::isRunning = false;

void AvWebserver::setup(AvBusWriter* writer) {
  if(instance == nullptr) instance = new AvWebserver(writer);
}

void AvWebserver::run(){
  Serial.println("Running server loop");
  while(isRunning) {
    instance->loop();
    delay(20);
  }
  Serial.println("Leaving server loop");
}

void AvWebserver::tearDown(){
  delete instance;
}

AvWebserver::AvWebserver(AvBusWriter* writer) : writer(writer) {
  Serial.println("Creating SSL certificate");
  certificate = new SSLCert();
  int createCertResult = createSelfSignedCert(
    *certificate,
    KEYSIZE_2048,
    "CN=avbus.fritz.box,O=JohannesDoll,C=DE",
    "20200101000000",
    "20310101000000"
  );
  //TODO: Store and reload certificate

  // Now check if creating that worked
  if (createCertResult != 0) {
    Serial.printf("Cerating certificate failed. Error Code = 0x%02X, check SSLCert.hpp for details", createCertResult);
    while(true) delay(500);
  } else {
    Serial.println("Successfully created certificate");
  }

  server = new HTTPSServer(certificate);
  ResourceNode* defaultNode = new ResourceNode("", "GET", &onNotFound);
  ResourceNode* rootNode = new ResourceNode("/", "GET", &onGetRoot);
  ResourceNode* postCommandNode = new ResourceNode("/api/v1/remote/command", "POST", &onPostCommand);

  server->setDefaultNode(defaultNode);
  server->registerNode(rootNode);
  server->registerNode(postCommandNode);

  Serial.println("Starting server...");
  server->start();
  if (server->isRunning()) {
    Serial.println("Server ready.");
    isRunning = true;
  }else{
    isRunning = false;
  }
}

AvWebserver::~AvWebserver() {
  if(server->isRunning()) server->stop();
  delete certificate;
  delete server;
}

void AvWebserver::loop(){
  server->loop();
}

void AvWebserver::onGetRoot(HTTPRequest* request, HTTPResponse * response) {
  response->setHeader(HttpHeader::CONTENT_TYPE, ContentType::APPLICATION_JSON);
  response->println("{ \"message\": \"Hello World\" }");
}

void AvWebserver::onNotFound(HTTPRequest* request, HTTPResponse* response) {
    response->setStatusCode(404);
    response->setStatusText("Page not found");
    response->println("404 Page not found");
}

void AvWebserver::onPostCommand(HTTPRequest* request, HTTPResponse* response) {
  if(instance == nullptr){
    response->setStatusCode(500);
    response->setStatusText("WebServer instance not initialized");
    response->println("500 WebServer instance not initialized");
    return;
  }

  const size_t capacity = JSON_OBJECT_SIZE(2) + 50;

  char* buffer = new char[capacity + 1];
  memset(buffer, 0, capacity+1);
  size_t readBytes = 0;
  while(!request->requestComplete() && readBytes < capacity){
    readBytes += request->readChars(buffer + readBytes, capacity - readBytes);
  }
  if(!request->requestComplete()){
    response->setStatusCode(413);
    response->setStatusText("Request entity to large");
    response->println("413 Request entity to large");
    delete[] buffer;
    return;
  }

  ArduinoJson::DynamicJsonDocument body(capacity);
  ArduinoJson::deserializeJson(body, buffer);

  if (body.isNull()) {
    response->setStatusCode(400);
    response->setStatusText("Invalid JSON. Must be an object.");
    response->println("400 Invalid JSON. Must be an object.");
    delete[] buffer;
    return;
  }

  if (VERBOSE) Serial.println("Checking Device");
  const char* KEY_DEVICE = "device";
  if (!body.containsKey(KEY_DEVICE) || !body[KEY_DEVICE].is<std::string>()) {
    response->setStatusCode(400);
    response->setStatusText("Invalid JSON. Missing or invalid value for 'device'");
    response->println("400 Invalid JSON. Missing or invalid value for 'device'");
    delete[] buffer;
    return;
  }

  if (VERBOSE) Serial.println("Reading Device");
  std::string deviceName = body[KEY_DEVICE];
  const Device* device = Device::getDeviceByName(deviceName);
  if (device == nullptr) {
    response->setStatusCode(400);
    response->setStatusText("Invalid 'device'");
    response->println("400 Invalid 'device'");
    delete[] buffer;
    return;
  }

  if (VERBOSE) Serial.println("Checking Command");
  const char* KEY_COMMAND = "command";
  if (!body.containsKey(KEY_COMMAND) || !body[KEY_COMMAND].is<std::string>()) {
    response->setStatusCode(400);
    response->setStatusText("Invalid JSON. Missing or invalid value for 'command'");
    response->println("400 Invalid JSON. Missing or invalid value for 'command'");
    delete[] buffer;
    return;
  }

  if (VERBOSE) Serial.println("Reading Command");
  std::string commandName = body[KEY_COMMAND];
  uint16_t command = (*device)[commandName];
  if (command == Device::Command::UNDEFINED) {
    response->setStatusCode(400);
    response->setStatusText("Invalid Command/Device combination.");
    response->println("400 Invalid Command/Device combination.");
    delete[] buffer;
    return;
  }

  if (VERBOSE) Serial.printf("Queuing command '%s' on device '%s'\n", commandName.c_str(), deviceName.c_str());
  Command combinedCommand((*device), command);
  instance->writer->queueCommand(combinedCommand);

  response->setStatusCode(202);
  delete[] buffer;
}