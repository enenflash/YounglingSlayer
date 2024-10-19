class Robot {
public:
  // defining obkects
  MotorController mc = MotorController(false);
  Adafruit_BNO055 bno = Adafruit_BNO055(55);
  IRSensor ir;

  PositionSystem ps;

  // defining variables
  float tilt = 0;
  float offset = 0;
  int direction = 0, strength = 0;

  float x = 0, y = 0;

  int lineValue;

private:
  /* :::::::: FUNCTIONS :::::::: */
  void getSensorData() {
    // READ IR
    ir.readData();
    direction = ir.getFilteredDirection();
    strength = ir.getFilteredStrength();

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
  
  void getBallXY(float& ballX, float& ballY) {
    ballX = cos(irAngles[direction]);
    ballY = sin(irAngles[direction]);
  };

public:
  void update() {
    // get sensor data using sensor classes
    getSensorData();
    Serial.println("Read sensors"); // code crashes without line between for some reason
    ps.update(tilt*PI/180);
  };

  void getBehindBall() {
    if (direction == 0) {
      x = 0, y = 0;
      return;
    }

    if (strength < 34) {
      getBallXY(x, y);
      return;
    }

    if (direction == 12) {
      x = 0, y = 1;
      return;
    }

    if (direction == 6) {
      x = 1, y = 0;
      return;
    }

    float movementAngle;

    float ballX = 0, ballY = 0;
    getBallXY(ballX, ballY);

    if (ballX < 0) {
      movementAngle = irAngles[direction] + PI/2;
    }
    else {
      movementAngle = irAngles[direction] - PI/2;
    }

    x = cos(movementAngle);
    y = sin(movementAngle);
  };

  void adjustSpeed() {
    if (lineValue != 0) {
      mc.setSpeed(LINE_SPEED);
      return;
    }

    if (irAngles[direction] > PI && irAngles[direction] < 2*PI) {
      mc.setSpeed(BALL_BEHIND_SPEED);
    }
    else {
      mc.setSpeed(100);
    }
  };

  void getOffset() {
    if (lineValue != 0 || !ps.reliable) {
      offset = 0;
      return;
    }

    if (ps.x < FIELD_WIDTH/2 - 20) {
      offset = -25;
    }
    else if (ps.x > FIELD_WIDTH/2 + 20) {
      offset = 25;
    }
    else {
      offset = 0;
    }

    // if (ps.x < 50) {
    //   offset = -30;
    // }
    // else if (FIELD_WIDTH - ps.x < 50) {
    //   offset = 0; // 30
    // }
    // else if (ps.x < (FIELD_WIDTH/2 - 20)) {
    //   offset = -20;
    // }
    // else if (ps.x > (FIELD_WIDTH/2 + 20)) {
    //   offset = 0; // 20
    // }
    // else {
    //   offset = 0;
    // }
  };

  // gets the goal x,y coordinants using ultrasonics
  void getGoalXY(float &x, float &y) {
    x = -(ps.x - GOAL_POS_X);
    y = ps.y - GOAL_POS_Y;
  };

  void targetGoal() {
    x = 0, y = 1;
    // if (!ps.reliable || direction != 12) {
    //   return;
    // }
    // getGoalXY(x, y);
  };

  // stays within the lines
  void logY() {
    if (lineValue == 0) {
      previousY = y;
    }
  };

  // stays within the lines
  void stopAtLine() {

    if (lineValue == 1 || lineValue == 4) {
      if(previousY < 0) x = 0, y = 1;
      else x = 0, y = -1;
    }

    else if (lineValue == 2) { // && (x < 0)
      if (ps.x < 40) {
        x = 1, y = 0;
      }
      else {
        // set speed to 100 as last resort to get out quick
        mc.setSpeed(100);
        x = -1, y = 0;
      }
    }
    else if (lineValue == 3) { //&& (x > 0)
      if (ps.x < 40) {
        // set speed to 100 as last resort to get out quick
        x = 1, y = 0;
      }
      else {
        mc.setSpeed(100);
        x = -1, y = 0;
      }
    }
  };

  void run() {
    // run motors using motor controller
    mc.runMotors(x, y, tilt, offset);
  };
};
