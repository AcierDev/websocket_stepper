#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "motor_control.h"

class WebSocketHandler {
 public:
  WebSocketHandler(MotorControl& motorControl);
  void setup();
  void loop();

 private:
  void handleWebSocketMessage(void* arg, uint8_t* data, size_t len);
  void handleLegacyCommands(const String& message);
  AsyncWebServer server;
  AsyncWebSocket ws;
  MotorControl& _motorControl;
};

#endif