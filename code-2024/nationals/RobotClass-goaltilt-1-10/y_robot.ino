class Robot {
public:
  MotorController mc = MotorController(false);
  PositionSystem ps;
  Adafruit_BNO055 bno = Adafruit_BNO055(55);
  IRSensor ir;

  float tilt = 0, offset = 0;
  int direction = 0, strength = 0, ballDist = 0;

  float x = 0, y = 0;

  String message;
  int lineValue;
  int linePos;

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
      message = Serial6.read();
      lineValue = message.substring(0, 1).toInt();
      linePos = message.substring(2).toInt();
    }
  };

  void getBallXY(float &x, float &y) {
    x = cos(irAngles[direction]) * ballDist;
    y = sin(irAngles[direction]) * ballDist;
  };

  void getAroundBall(float &ballX, float &ballY) {
    if (direction == 6 && strength > 38) {
      ballX = -1, ballY = 0;
      return;
    }

    if (irAngles[direction] == PI || irAngles[direction] == 2*PI) {
      ballX = 0, ballY = -1;
    }

    if (irAngles[direction] > PI && irAngles[direction] != 2*PI) {
      if (irAngles[direction] == 3*PI/2) {
        if (ps.x < FIELD_WIDTH/2) {
          ballX += ROBOT_TO_BALL_SIDE;
        }
        else {
          ballX -= ROBOT_TO_BALL_SIDE;
        }
      }
      else if (irAngles[direction] > 3*PI/2) {
        ballX -= ROBOT_TO_BALL_SIDE;
      }
      else {
        ballX += ROBOT_TO_BALL_SIDE;
      }
    }
    // else if (irAngles[direction] == PI || irAngles[direction] == 2*PI) {
    //   ballY -= ROBOT_TO_BALL_BACK * 2;
    // }
    else {
      ballY -= ROBOT_TO_BALL_BACK;
    }
  };

  bool ballBehind() {
    return (PI < irAngles[direction] && irAngles[direction] < 2*PI);
  };

public:
  void update() {
    // get sensor data using sensor classes
    getSensorData();
    ps.update(tilt*PI/180);
  };

  void drive(float x, float y) {
    mc.runMotors(x, y, tilt, 0);
  };

  void stop() {
    mc.stopMotors();
  };

  void getGoalXY(float &x, float &y) {
    x = -(ps.x - GOAL_POS_X);
    y = ps.y - GOAL_POS_Y;
  };

  void goalTilt(int angle) {
    if (ps.x < FIELD_WIDTH/2 - GOAL_TILT_BOUND) {
      offset = -angle;
    }
    else if (ps.x > FIELD_WIDTH/2 + GOAL_TILT_BOUND) {
      offset = angle;
    }
    else {
      offset = 0;
    }
  };

  void targetGoal() {
    x = 0, y = 1;
    // check if history is consistent for either ultrasonic
    if (ps.reliable) {
      getGoalXY(x, y);
      //goalTilt(20);
    }
  };

  void bashBall() {
    getBallXY(x, y);
    if (direction > PI && direction != 2*PI) {
      getAroundBall(x, y);
    }
  };

  void getBehindBall() {
    // get XY based on ball position
    getBallXY(x, y);
    getAroundBall(x, y);
    mc.setSpeed(100);
  };

  void adjustSpeed() {
    if (irAngles[direction] > PI && irAngles[direction] < 2*PI) {
      mc.setSpeed(BALL_BEHIND_SPEED);
    }
    else {
      mc.setSpeed(100);
    }
  };

  void stopAtLine() {
    if (lineValue != 0) {
      mc.setSpeed(LINE_SPEED);
    }
    if (lineValue == 1 || lineValue == 4) {
      if (linePos < 15 && linePos > -15) {
        if (ps.y < FIELD_LENGTH/2) { // front
          x = 0, y = -1;
        }
        else if (ps.y > FIELD_LENGTH/2) {
          x = 0, y = 1;
        }
      }
      else if (lineValue == 1) {
        x = 0, y = -1;
      }
      else if (lineValue == 4) {
        x = 0, y = 1;
      }
    }
    else if (lineValue == 2 || lineValue == 3) {
      if (linePos < 15 && linePos > -15) {
        if (ps.y < FIELD_WIDTH/2) { // left
          x = 1, y = 0;
        }
        else if (ps.y > FIELD_WIDTH/2) {
          x = -1, y = 0;
        }
      }
      else if (lineValue == 2) {
        x = 1, y = 0;
      }
      else if (lineValue == 3) {
        x = -1, y = 0;
      }
    }
  };

  void manualDefendGoal() {
    x = 0, y = 0;

    if (ps.x < FIELD_WIDTH/2 - GOAL_RANGE) {
      x = 1;
    }
    else if (ps.x > FIELD_WIDTH/2 + GOAL_RANGE) {
      x = -1;
    }

    if (ps.y < GOAL_DEFENCE_Y - GOAL_RANGE) {
      y = -1;
    }
    else if (ps.y > GOAL_DEFENCE_Y + GOAL_RANGE) {
      y = 1;
    }
  };

  void run() {
    // run motors using motor controller
    mc.runMotors(x, y, tilt, offset);
  };
};