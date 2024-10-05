class Robot {
public:
  MotorController mc = MotorController(false);
  PositionSystem ps;
  Adafruit_BNO055 bno = Adafruit_BNO055(55);
  IRSensor ir;

  float tilt = 0, offset = 0;
  int direction = 0, strength = 0, ballDist = 0;

  float x = 0, y = 0;

  int lineValue;

private:
  void getSensorData() {
    // READ IR
    ir.readData();
    direction = ir.getFilteredDirection();
    strength = ir.getRawStrength();
    ballDist = 100-strength;

    // READ IMU
    sensors_event_t event;
    bno.getEvent(&event);
    tilt = event.orientation.x;

    // convert to unit circle tilt
    tilt = 360 - tilt;

    // get values from line sensor
    if (Serial6.available()) {
      lineValue = Serial6.read() - 1;
    }
  };

  void getXY(int direction, int ballDist) {
    if (direction == 0) {
      x = 0, y = 0;
      return;
    }
    else if (direction == 12) {
      x = 0, y = 1;
      return;
    }

    x = cos(irAngles[direction]) * ballDist;
    y = sin(irAngles[direction]) * ballDist;

    if (irAngles[direction] > PI && irAngles[direction] != 2*PI) {
      if (irAngles[direction] == 3*PI/2) {
        // if (ps.x < FIELD_WIDTH) {
        //   x += ROBOT_TO_BALL_SIDE;
        // }
        // else {
        //   x -= ROBOT_TO_BALL_SIDE;
        // }
        x += ROBOT_TO_BALL_SIDE;
      }
      else if (irAngles[direction] > 3*PI/2) {
        x -= ROBOT_TO_BALL_SIDE;
      }
      else {
        x += ROBOT_TO_BALL_SIDE;
      }
    }
    else if (irAngles[direction] == PI || irAngles[direction] == 2*PI) {
      y -= ROBOT_TO_BALL_BACK * 2;
    }
    else {
      y -= ROBOT_TO_BALL_BACK;
    }
  };

public:

  void update() {
    // get sensor data using sensor classes
    getSensorData();
    ps.update(tilt*PI/180);

    // Serial.print("x: ");
    // Serial.print(x);
    // Serial.print(" y: ");
    // Serial.println(y);
  };

  bool ballBehind() {
    return (PI < irAngles[direction] && irAngles[direction] < 2*PI);
  };

  void stopAtLine() {
    if (lineValue != 0) {
      mc.setSpeed(50);
    }
    if ((lineValue == 1) && (y > 0)) {
      y = y * -1;
    }
    else if ((lineValue == 2) && (x < 0)) {
      x = x * -1;
    }
    else if ((lineValue == 3) && (x > 0)) {
      x = x * -1;
    }
    else if ((lineValue == 4) && (y < 0)) {
      y = y * -1;
    }
    Serial.print("Line Value: ");
    Serial.print(lineValue);
    Serial.print(" x: ");
    Serial.print(x);
    Serial.print(" y: ");
    Serial.println(y);
  };

  void goalTilt() {
    if (direction != 12) {
      offset = 0;
      return;
    }

    if (x < FIELD_WIDTH - 25) {
      offset = -20;
    }
    else if (x > FIELD_WIDTH + 25) {
      offset = 20;
    }
    else {
      offset = 0;
    }
  }

  void getBehindBall() {
    // get XY based on ball position
    getXY(direction, ballDist);
    mc.setSpeed(100);
  };

  void testLine() {
    switch (lineValue) {
      case 1:
        x = -1, y = -1;
        break;
      case 2:
        x = 1, y = 0;
        break;
      case 3:
        x = 0, y = -1;
        break;
      case 4:
        x = 1, y = 1;
        break;
      default:
        x = 0, y = 1;
        break;
    }

    Serial.print("test line: ");
    Serial.println(lineValue);

    // run motors using motor controller
    mc.runMotors(x, y, 0);
  };

  void adjustSpeed() {
    if (irAngles[direction] >= PI) {
      mc.setSpeed(60);
    }
    else {
      mc.setSpeed(100);
    }
  };

  void run() {
    // run motors using motor controller
    mc.runMotors(x, y, tilt, offset);
  }
};