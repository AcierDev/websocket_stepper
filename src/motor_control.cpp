#include "motor_control.h"

#include <algorithm>  // For std::min and std::max

MotorControl* MotorControl::instance = nullptr;

MotorControl::MotorControl(int pulsePin1, int dirPin1, int pulsePin2,
                           int dirPin2, int endstopPin1, int endstopPin2)
    : stepperY(1, pulsePin1, dirPin1),
      stepperX(1, pulsePin2, dirPin2),
      _endstopPin1(endstopPin1),
      _endstopPin2(endstopPin2) {
  instance = this;  // Store instance pointer for timer callback
}

void IRAM_ATTR MotorControl::onXMoveTimer() { instance->_xMoveReady = true; }

void MotorControl::setup() {
  // Configure endstops with Bounce2
  endstop1.attach(_endstopPin1, INPUT_PULLUP);
  endstop2.attach(_endstopPin2, INPUT_PULLUP);
  endstop1.interval(DEBOUNCE_MS);
  endstop2.interval(DEBOUNCE_MS);

  pinMode(SUCTION_PIN, OUTPUT);
  pinMode(EXTENSION_PIN, OUTPUT);

  // Initialize relays to OFF state (HIGH due to inverted logic)
  digitalWrite(SUCTION_PIN, HIGH);
  digitalWrite(EXTENSION_PIN, HIGH);
  suctionState = false;
  extensionState = false;

  // Set parameters for both motors
  stepperY.setMaxSpeed(20000);
  stepperY.setAcceleration(20000);
  stepperY.setSpeed(20000);
  stepperY.setMinPulseWidth(3);

  stepperX.setMaxSpeed(20000);
  stepperX.setAcceleration(20000);
  stepperX.setSpeed(20000);
  stepperX.setMinPulseWidth(3);

  // Initialize hardware timer
  xMoveTimer = timerBegin(0, 80, true);  // Timer 0, prescaler 80, count up
  timerAttachInterrupt(xMoveTimer, &onXMoveTimer, true);
  timerAlarmWrite(xMoveTimer, 150000, false);  // 150ms (in microseconds)
}

void IRAM_ATTR MotorControl::runMotors() {
  stepperY.run();

  if (_delayedXMove && _xMoveReady) {
    _delayedXMove = false;
    _xMoveReady = false;
    stepperX.moveTo(_xMoveSteps);
  }

  stepperX.run();
  endstop1.update();
  endstop2.update();
}

void MotorControl::moveMotor(int motorNum, long steps) {
  if (abs(steps) > MAX_STEPS) {
    return;
  }

  // Get current position in steps
  long currentPos =
      (motorNum == 1) ? stepperY.currentPosition() : stepperX.currentPosition();

  // Calculate target position after move
  long targetPos = currentPos + steps;

  // Convert to coordinates for limit checking
  double targetCoord = targetPos / STEPS_PER_UNIT;

  // Check limits based on motor
  if (motorNum == 1) {  // Y axis
    if (targetCoord < 0 || targetCoord > 40.0) {
      return;
    }
  } else if (motorNum == 2) {  // X axis
    if (targetCoord < 0 || targetCoord > 38.0) {
      return;
    }
  }

  if (motorNum == 1) {
    stepperY.move(steps);
  } else if (motorNum == 2) {
    stepperX.move(steps);
  }
}

void MotorControl::homeMotor(int motorNum) {
  // This single-motor homing function can remain as a backup
  AccelStepper& stepper = (motorNum == 1) ? stepperY : stepperX;
  int endstopPin = (motorNum == 1) ? _endstopPin1 : _endstopPin2;

  // Save current speed settings
  float currentSpeed = stepper.maxSpeed();
  float currentAccel = stepper.acceleration();

  // Set slower speed for homing
  stepper.setMaxSpeed(2000);       // Slower max speed for homing
  stepper.setAcceleration(20000);  // Gentler acceleration for homing

  stepper.moveTo(-1000000);

  int consecutiveHighs = 0;
  while (consecutiveHighs < 7) {
    if (digitalRead(endstopPin) == HIGH) {
      consecutiveHighs++;
    } else {
      consecutiveHighs = 0;  // Reset counter if we get a LOW reading
    }
    stepper.run();
  }

  stepper.stop();
  stepper.setCurrentPosition(0);

  // Move away from endstop slowly
  stepper.moveTo(100);
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }

  // Restore original speed settings
  stepper.setMaxSpeed(currentSpeed);
  stepper.setAcceleration(currentAccel);
}

void MotorControl::homeMotors() {
  // Save current speeds
  float currentSpeedY = stepperY.maxSpeed();
  float currentAccelY = stepperY.acceleration();
  float currentSpeedX = stepperX.maxSpeed();
  float currentAccelX = stepperX.acceleration();

  // Set homing speeds
  stepperY.setMaxSpeed(2000);
  stepperY.setAcceleration(20000);
  stepperX.setMaxSpeed(2000);
  stepperX.setAcceleration(20000);

  // Start moving both motors towards home
  stepperY.moveTo(-1000000);
  stepperX.moveTo(-1000000);

  bool yHomed = false;
  bool xHomed = false;
  int consecutiveHighsY = 0;
  int consecutiveHighsX = 0;

  // Run both motors until both hit their endstops
  while (!yHomed || !xHomed) {
    if (!yHomed) {
      if (digitalRead(_endstopPin1) == HIGH) {
        consecutiveHighsY++;
        if (consecutiveHighsY >= 7) {
          stepperY.stop();
          stepperY.setCurrentPosition(0);
          yHomed = true;
          Serial.println("Y axis homed");
        }
      } else {
        consecutiveHighsY = 0;
        stepperY.run();
      }
    }

    if (!xHomed) {
      if (digitalRead(_endstopPin2) == HIGH) {
        consecutiveHighsX++;
        if (consecutiveHighsX >= 7) {
          stepperX.stop();
          stepperX.setCurrentPosition(0);
          xHomed = true;
          Serial.println("X axis homed");
        }
      } else {
        consecutiveHighsX = 0;
        stepperX.run();
      }
    }
  }

  // Move both axes away from endstops
  stepperY.moveTo(100);
  stepperX.moveTo(100);

  while (stepperY.distanceToGo() != 0 || stepperX.distanceToGo() != 0) {
    stepperY.run();
    stepperX.run();
  }

  // Restore original speeds
  stepperY.setMaxSpeed(currentSpeedY);
  stepperY.setAcceleration(currentAccelY);
  stepperX.setMaxSpeed(currentSpeedX);
  stepperX.setAcceleration(currentAccelX);
}

void MotorControl::moveToPosition(int motorNum, long position) {
  // Convert steps to coordinates for limit checking
  double coord = position / STEPS_PER_UNIT;

  // Apply limits based on motor number
  if (motorNum == 1) {  // Y axis
    if (coord < 0 || coord > 40.0) {
      return;
    }
  } else if (motorNum == 2) {  // X axis
    if (coord < 0 || coord > 38.0) {
      return;
    }
  }

  if (abs(position) > MAX_STEPS) {
    return;
  }

  if (motorNum == 1) {
    stepperY.moveTo(position);
  } else if (motorNum == 2) {
    stepperX.moveTo(position);
  }
}

void MotorControl::togglePattern(int motorNum) {
  if (motorNum == 1) {
    if (_pattern1Running) {
      _pattern1Running = false;
      _pattern1Step = 0;
      stepperY.stop();
    } else {
      _pattern1Running = true;
      _pattern1Step = 0;
      _lastPattern1Move = millis();
      moveToPosition(1, 1000);
    }
  } else if (motorNum == 2) {
    if (_pattern2Running) {
      _pattern2Running = false;
      _pattern2Step = 0;
      stepperX.stop();
    } else {
      _pattern2Running = true;
      _pattern2Step = 0;
      _lastPattern2Move = millis();
      moveToPosition(2, 1000);
    }
  }
}

void MotorControl::motorPattern() { runPattern(); }

void MotorControl::startPattern(int rows, int cols, double startX,
                                double startY, double endX, double endY,
                                double pickupX, double pickupY, float speed,
                                float acceleration) {
  // Apply hard limits to all coordinates
  startX = std::min(std::max(startX, 0.0), 38.0);
  endX = std::min(std::max(endX, 0.0), 38.0);
  startY = std::min(std::max(startY, 0.0), 40.0);
  endY = std::min(std::max(endY, 0.0), 40.0);
  pickupX = std::min(std::max(pickupX, 0.0), 38.0);
  pickupY = std::min(std::max(pickupY, 0.0), 40.0);

  // Store pattern parameters
  patternSpeed = speed;
  patternAccel = acceleration;
  patternRows = rows;
  patternCols = cols;
  gridStartX = startX;
  gridStartY = startY;
  gridWidth = endX;   // Store the end X coordinate
  gridLength = endY;  // Store the end Y coordinate
  pickupPosition = Point(pickupX, pickupY);

  // Generate the pattern using start and end coordinates directly
  currentPattern =
      patternGen.generatePattern(rows, cols, startX, startY, endX, endY);

  // Verify all generated points are within limits
  for (const Point& point : currentPattern) {
    if (point.x < 0 || point.x > 38.0 || point.y < 0 || point.y > 40.0) {
      // If any point is out of bounds, stop pattern before it starts
      patternActive = false;
      currentPatternIndex = 0;
      currentPattern.clear();
      Serial.println("Pattern cancelled - points out of bounds");
      return;
    }
  }

  // Initialize pattern execution
  currentPatternIndex = 0;
  patternActive = true;
  patternState = PatternState::IDLE;

  // Update motor speeds
  stepperY.setMaxSpeed(speed);
  stepperX.setMaxSpeed(speed);
  stepperY.setAcceleration(acceleration);
  stepperX.setAcceleration(acceleration);
}

void MotorControl::stopPattern() {
  patternActive = false;
  patternState = PatternState::IDLE;
  currentPatternIndex = 0;
  // Stop any ongoing movement
  stepperY.stop();
  stepperX.stop();
}

void MotorControl::runPattern() {
  if (!patternActive || currentPattern.empty()) {
    return;
  }

  if (stepperY.distanceToGo() != 0 || stepperX.distanceToGo() != 0) {
    return;
  }

  switch (patternState) {
    case PatternState::IDLE:
      gotoPosition(pickupPosition.x, pickupPosition.y, patternSpeed,
                   patternAccel);
      patternState = PatternState::MOVING_TO_PICKUP;
      break;

    case PatternState::MOVING_TO_PICKUP:
      pick();
      patternState = PatternState::PICKING;
      delay(200);
      break;

    case PatternState::PICKING: {
      Point placePoint = currentPattern[currentPatternIndex];
      gotoPosition(placePoint.x, placePoint.y, patternSpeed, patternAccel);
      patternState = PatternState::MOVING_TO_PLACE;
    } break;

    case PatternState::MOVING_TO_PLACE:
      place();
      patternState = PatternState::PLACING;
      break;

    case PatternState::PLACING: {
      // Check if we're in the last row (closest to pickup)
      bool isLastRow =
          currentPatternIndex >= (currentPattern.size() - patternCols);

      if (isLastRow) {
        delay(300);  // Additional delay for last row
      }

      currentPatternIndex++;

      if (currentPatternIndex >= currentPattern.size()) {
        patternActive = false;
        currentPatternIndex = 0;
      } else {
        patternState = PatternState::IDLE;
      }
    } break;
  }
}

long MotorControl::coordToSteps(double coord) {
  // Convert coordinates to steps based on your machine's configuration
  // You'll need to adjust this based on your stepper motors' steps per unit
  return (long)(coord * STEPS_PER_UNIT);  // STEPS_PER_UNIT needs to be defined
}

void MotorControl::gotoPosition(double x, double y, float speed,
                                float acceleration) {
  // Get current positions in steps
  long currentYSteps = stepperY.currentPosition();
  long currentXSteps = stepperX.currentPosition();

  // Get current positions in coordinates
  double currentX = currentXSteps / STEPS_PER_UNIT;
  double currentY = currentYSteps / STEPS_PER_UNIT;

  // Calculate relative movement in coordinates
  double deltaX = x - currentX;
  double deltaY = y - currentY;

  // Clamp the target position to stay within limits
  double targetX = std::min(std::max(currentX + deltaX, 0.0), 38.0);
  double targetY = std::min(std::max(currentY + deltaY, 0.0), 40.0);

  // Convert clamped coordinates to steps
  long xSteps = coordToSteps(targetX);
  long ySteps = coordToSteps(targetY);

  // Set speeds and accelerations
  stepperY.setMaxSpeed(speed);
  stepperX.setMaxSpeed(speed);
  stepperY.setAcceleration(acceleration);
  stepperX.setAcceleration(acceleration);

  // Check if we're moving to pickup position
  bool movingToPickup =
      (patternState == PatternState::IDLE && abs(x - pickupPosition.x) < 0.1 &&
       abs(y - pickupPosition.y) < 0.1);

  if (movingToPickup) {
    // Start Y movement immediately
    stepperY.moveTo(ySteps);

    // Set up delayed X movement using hardware timer
    _delayedXMove = true;
    _xMoveReady = false;
    _xMoveSteps = xSteps;
    timerRestart(xMoveTimer);  // Reset and start the timer
    timerAlarmEnable(xMoveTimer);
  } else {
    // For all other movements, move both axes simultaneously
    _delayedXMove = false;
    stepperY.moveTo(ySteps);
    stepperX.moveTo(xSteps);
  }
}

void MotorControl::setSuction(bool state) {
  suctionState = state;
  digitalWrite(SUCTION_PIN, !state);
}

void MotorControl::setExtension(bool state) {
  extensionState = state;
  digitalWrite(EXTENSION_PIN, !state);
}

void MotorControl::pick() {
  setSuction(true);
  delay(50);

  setExtension(true);
  delay(300);

  setExtension(false);
}

void MotorControl::place() {
  setExtension(true);
  delay(100);

  setSuction(false);
  delay(100);

  setExtension(false);
  delay(100);
}

void MotorControl::runStressTest() {
  if (!stressTestActive) {
    return;
  }

  if (stepperY.distanceToGo() != 0 || stepperX.distanceToGo() != 0) {
    return;
  }

  switch (patternState) {
    case PatternState::IDLE:
      if (stressTestCount >= STRESS_TEST_ITERATIONS) {
        stressTestActive = false;
        patternState = PatternState::IDLE;
        return;
      }
      gotoPosition(pickupPosition.x, pickupPosition.y, 10000, 10000);
      patternState = PatternState::MOVING_TO_PICKUP;
      break;

    case PatternState::MOVING_TO_PICKUP:
      pick();
      patternState = PatternState::PICKING;
      break;

    case PatternState::PICKING:
      gotoPosition(stressTestPlacePosition.x, stressTestPlacePosition.y, 10000,
                   10000);
      patternState = PatternState::MOVING_TO_PLACE;
      break;

    case PatternState::MOVING_TO_PLACE:
      place();
      patternState = PatternState::PLACING;
      break;

    case PatternState::PLACING:
      stressTestCount++;
      patternState = PatternState::IDLE;
      break;
  }
}

void MotorControl::stopStressTest() {
  stressTestActive = false;
  stressTestCount = 0;
  patternState = PatternState::IDLE;
  stepperY.stop();
  stepperX.stop();
}