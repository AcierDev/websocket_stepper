#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "motor_control.h"
#include "webpage.h"
#include "websocket_handler.h"

const char* ssid = "Everwood";
const char* password = "Everwood-Staff";

const int pulsePin1 = 27;
const int dirPin1 = 14;
const int pulsePin2 = 25;
const int dirPin2 = 26;
const int endstopPin1 = 32;
const int endstopPin2 = 33;

MotorControl motorControl(pulsePin1, dirPin1, pulsePin2, dirPin2, endstopPin1,
                          endstopPin2);
WebSocketHandler wsHandler(motorControl);

void setup();
void loop();
#line 26 "/Users/steelebenjamin/Documents/Everwood/Code/websocket_stepper/src/websocket_stepper.ino"
void setup() {
  Serial.begin(115200);

  motorControl.setup();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  wsHandler.setup();
}

void loop() {
  wsHandler.loop();
  motorControl.runMotors();
  motorControl.motorPattern();
  motorControl.runStressTest();
}