#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <AccelStepper.h>
#include <Arduino.h>
#include <Bounce2.h>

#include "pattern_generator.h"

class MotorControl {
 public:
  MotorControl(int pulsePin1, int dirPin1, int pulsePin2, int dirPin2,
               int endstopPin1, int endstopPin2);
  void setup();
  void runMotors();
  void moveMotor(int motorNum, long steps);
  void homeMotor(int motorNum);
  void moveToPosition(int motorNum, long position);
  void motorPattern();
  void togglePattern(int motorNum);
  bool isPatternRunning(int motorNum) {
    return (motorNum == 1) ? _pattern1Running : _pattern2Running;
  }
  void startPattern(int rows, int cols, double startX, double startY,
                    double width, double length, double pickupX, double pickupY,
                    float speed, float acceleration);
  void stopPattern();
  bool isPatternActive() const { return patternActive; }
  void gotoPosition(double x, double y, float speed, float acceleration);
  static constexpr float STEPS_PER_UNIT =
      254.0;  // Adjust based on your mechanics
  void setSuction(bool state);
  void setExtension(bool state);
  bool getSuctionState() const { return suctionState; }
  bool getExtensionState() const { return extensionState; }
  void pick();
  void place();
  void runStressTest();
  bool isStressTestRunning() const { return stressTestActive; }
  void stopStressTest();

  bool stressTestActive = false;
  int stressTestCount = 0;
  static const int STRESS_TEST_ITERATIONS = 100;
  Point stressTestPlacePosition = Point(5, 5);  // 5 inches from home

  enum class PatternState {
    IDLE,
    MOVING_TO_PICKUP,
    PICKING,
    MOVING_TO_PLACE,
    PLACING
  };
  PatternState patternState = PatternState::IDLE;
  Point pickupPosition;  // Replace the const PICKUP_POSITION

 private:
  AccelStepper stepper1;
  AccelStepper stepper2;
  int _endstopPin1;
  int _endstopPin2;
  static const long MAX_STEPS = 100000;
  bool _pattern1Running = false;
  bool _pattern2Running = false;
  unsigned long _lastPattern1Move = 0;
  unsigned long _lastPattern2Move = 0;
  int _pattern1Step = 0;
  int _pattern2Step = 0;
  PatternGenerator patternGen;
  std::vector<Point> currentPattern;
  size_t currentPatternIndex;
  bool patternActive = false;
  void runPattern();

  // Pattern parameters
  int patternRows = 0;
  int patternCols = 0;
  double gridStartX = 0;
  double gridStartY = 0;
  double gridWidth = 0;
  double gridLength = 0;
  double pieceSize = 0;
  double borderSize = 0;
  long coordToSteps(double coord);  // Add the function declaration
  static const int SUCTION_PIN = 23;
  static const int EXTENSION_PIN = 22;
  bool suctionState = false;
  bool extensionState = false;

  Bounce2::Button endstop1;
  Bounce2::Button endstop2;

  // Homing parameters
  static const int HOMING_SPEED = 1000;   // Steps per second
  static const int APPROACH_SPEED = 200;  // Slower speed for final approach
  static const int BACKOFF_DISTANCE =
      100;                            // Steps to back off after hitting endstop
  static const int DEBOUNCE_MS = 10;  // Debounce time in milliseconds

  float patternSpeed = 10000;  // Default speed
  float patternAccel = 10000;  // Default acceleration
};

#endif