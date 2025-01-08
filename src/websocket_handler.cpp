#include "websocket_handler.h"

#include "webpage.h"

WebSocketHandler::WebSocketHandler(MotorControl& motorControl)
    : server(80), ws("/ws"), _motorControl(motorControl) {}

void WebSocketHandler::setup() {
  // Attach WebSocket handler
  ws.onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client,
                    AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_DATA) {
      data[len] = 0;
      handleWebSocketMessage(arg, data, len);
    }
  });

  server.addHandler(&ws);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
}

void WebSocketHandler::loop() { ws.cleanupClients(); }

void WebSocketHandler::handleWebSocketMessage(void* arg, uint8_t* data,
                                              size_t len) {
  AwsFrameInfo* info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    String message = String((char*)data);
    Serial.print("Received: ");
    Serial.println(message);

    // Handle all commands as strings
    if (message.startsWith("moveY ")) {
      int steps = message.substring(6).toInt();
      _motorControl.moveMotor(1, steps);
    } else if (message.startsWith("moveX ")) {
      int steps = message.substring(6).toInt();
      _motorControl.moveMotor(2, steps);
    } else if (message == "home") {
      _motorControl.homeMotors();
    } else if (message == "homeY") {
      _motorControl.homeMotor(1);
    } else if (message == "homeX") {
      _motorControl.homeMotor(2);
    } else if (message == "patternY") {
      _motorControl.togglePattern(1);
    } else if (message == "patternX") {
      _motorControl.togglePattern(2);
    } else if (message.startsWith("pattern start ")) {
      // Format: "pattern start <rows> <cols> <startX> <startY> <endX> <endY>
      // <pickupX> <pickupY> <speed> <accel>"
      String params = message.substring(14);
      float values[10];  // Changed from 12 to 10 parameters
      int paramIndex = 0;
      int lastSpace = -1;

      // Parse all parameters
      while (paramIndex < 10 &&
             lastSpace < (int)params.length()) {  // Changed from 12 to 10
        int nextSpace = params.indexOf(' ', lastSpace + 1);
        if (nextSpace == -1) nextSpace = params.length();

        String value = params.substring(lastSpace + 1, nextSpace);
        values[paramIndex] = value.toFloat();
        Serial.printf("Parsed parameter %d: %s -> %f\n", paramIndex,
                      value.c_str(), values[paramIndex]);

        lastSpace = nextSpace;
        paramIndex++;
      }

      if (paramIndex == 10) {  // Changed from 12 to 10
        _motorControl.startPattern(values[0], values[1],  // rows, cols
                                   values[2], values[3],  // startX, startY
                                   values[4], values[5],  // endX, endY
                                   values[6], values[7],  // pickupX, pickupY
                                   values[8],
                                   values[9]);  // speed, acceleration
      }
    } else if (message == "pattern stop") {
      _motorControl.stopPattern();
      ws.textAll("Pattern stopped");
    } else if (message == "suction on") {
      _motorControl.setSuction(true);
      ws.textAll("Suction ON");
    } else if (message == "suction off") {
      _motorControl.setSuction(false);
      ws.textAll("Suction OFF");
    } else if (message == "extension on") {
      _motorControl.setExtension(true);
      ws.textAll("Extension ON");
    } else if (message == "extension off") {
      _motorControl.setExtension(false);
      ws.textAll("Extension OFF");
    } else if (message.startsWith("goto ")) {
      // Format: "goto <x> <y> <speed> <accel>"
      String params = message.substring(5);
      float values[4];
      int paramIndex = 0;
      int lastSpace = -1;

      // Parse all parameters
      while (paramIndex < 4 && lastSpace < (int)params.length()) {
        int nextSpace = params.indexOf(' ', lastSpace + 1);
        if (nextSpace == -1) nextSpace = params.length();

        String value = params.substring(lastSpace + 1, nextSpace);
        values[paramIndex] = value.toFloat();

        lastSpace = nextSpace;
        paramIndex++;
      }

      if (paramIndex == 4) {
        _motorControl.gotoPosition(values[0], values[1], values[2], values[3]);
      }
    } else if (message == "pick") {
      _motorControl.pick();
    } else if (message == "place") {
      _motorControl.place();
    } else if (message == "stress test start") {
      _motorControl.stressTestCount = 0;
      _motorControl.stressTestActive = true;
      _motorControl.patternState = MotorControl::PatternState::IDLE;
      ws.textAll("Stress test started");
    } else if (message == "stress test stop") {
      _motorControl.stopStressTest();
      ws.textAll("Stress test stopped");
    }
  }
}