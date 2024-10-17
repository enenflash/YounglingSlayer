/* :::::::: MAIN ROBOT CLASS :::::::: */
class Robot {
public:
  // defining obkects
  MotorController mc = MotorController(false);
  PositionSystem ps;
  Adafruit_BNO055 bno = Adafruit_BNO055(55);
  IRSensor ir;
  Bluetooth Bluetoothcomm;

  // defining variables
  float tilt = 0, offset = 0;
  int direction = 0, strength = 0, ballDist = 0;

  float x = 0, y = 0;

  int lineValue;

  String teamData;
  int teamX, teamY, teamDir, teamStr;

private:
  /* :::::::: FUNCTIONS :::::::: */
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

  void getBallXY(float &ballX, float &ballY) {
    ballX = cos(irAngles[direction]) * ballDist;
    ballY = sin(irAngles[direction]) * ballDist;
  };

  void getAroundBall(float &x, float &y) {
    if (direction == 6 && strength > 36) {
      if (ps.x < FIELD_WIDTH/2 || abs(ps.x - (FIELD_WIDTH - 2 * ULTRASONIC_TO_ROBOT - ps.x)) < 15 ) { 
        x = 1, y = 0;
      }
      else {
        x = -1, y = 0;
      }
      return;
    }

    if (direction == 3 || direction == 9) {
      x = 0, y = -1;
      return;
    }

    if (irAngles[direction] > PI && irAngles[direction] < 2*PI) {
      if (direction == 6) {
        if (ps.x < FIELD_WIDTH/2) {
          x += RTB_SIDE;
        }
        else {
          x -= RTB_SIDE;
        }
      }
      else if (irAngles[direction] > 3*PI/2) {
        x -= RTB_SIDE;
      }
      else {
        x += RTB_SIDE;
      }
    }
    // else if (irAngles[direction] == PI || irAngles[direction] == 2*PI) {
    //   y -= RTB_BACK * 2;
    // }
    else {
      y -= RTB_BACK;
    }
  };

  // checks if the ball is behind the robot
  bool ballBehind() {
    return (PI < irAngles[direction] && irAngles[direction] < 2*PI);
  };

public:
  void update() {
    // get sensor data using sensor classes
    getSensorData();
    ps.update(tilt*PI/180);
  };

  void updateBluetooth() {
    // send bluetooth data
    Bluetoothcomm.Send_Data(ps.x, ps.y, direction, ballDist);

    // recieve bluetooth data
    teamData = Bluetoothcomm.Get_Data();
    
    if (teamData != "NONE") {
      int comma_index = teamData.indexOf(",");
      int second_comma_index = teamData.indexOf(",", comma_index + 1);
      int third_comma_index = teamData.indexOf(",", second_comma_index + 1);
      
      if (comma_index != -1 && second_comma_index != -1 && third_comma_index != -1) {
        teamX = teamData.substring(0, comma_index).toInt();
        teamY = teamData.substring(comma_index + 1, second_comma_index).toInt();
        teamDir = teamData.substring(second_comma_index + 1, third_comma_index).toInt();
        teamStr = teamData.substring(third_comma_index + 1).toInt();
      }
      else {
        teamX = 0, teamY = 0, teamDir = 0, teamStr = 0;
      }
    }
    else {
      teamX = 0, teamY = 0, teamDir = 0, teamStr = 0;
    }
  };

  // stop all motors
  void drive(float x, float y) {
    mc.runMotors(x, y, tilt, 0);
  };

  void stop() {
    mc.stopMotors();
  };

  // gets the goal x,y coordinants using ultrasonics
  void getGoalXY(float &x, float &y) {
    x = -(ps.x - GOAL_POS_X);
    y = ps.y - GOAL_POS_Y;
  };

  // calculates robot's tilt towards the goals
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
    if (ps.reliable) {
      getGoalXY(x, y);
      //goalTilt(20);
    }
  };

  // hitting ball to side as last resort to avoid goal
  void bashBall() {
    getBallXY(x, y);
    if (irAngles[direction] > PI && direction != 9) {
      getAroundBall(x, y);
    }
  };

  // gets behind the ball to catch the ball
  void getBehindBall() {
    // get XY based on ball position
    getBallXY(x, y);
    getAroundBall(x, y);
  };

  // ajusts the robot's speed
  void adjustSpeed() {
    if (direction == 3 || direction == 9) {
      mc.setSpeed(BALL_SIDE_SPEED);
    }
    else if (irAngles[direction] > PI && irAngles[direction] < 2*PI && strength > SLOW_DOWN_STRENGTH) {
      mc.setSpeed(BALL_BEHIND_SPEED);
    }
    else {
      mc.setSpeed(100);
    }
  };

  // stays within the lines
  void stopAtLine() {
    if (lineValue != 0) {
      mc.setSpeed(LINE_SPEED);
    }

    if (lineValue == 1) { // && y > 0
      x = 0, y = -1;
    }
    else if (lineValue == 4) { // && y < 0
      x = 0, y = 1;
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
        mc.setSpeed(100);
        x = 1, y = 0;
      }
      else {
        x = -1, y = 0;
      }
    }
  };

  // moves infront of goal to defend
  void manualDefendGoal() {
    getBallXY(x, y);
    
    if (strength > GOAL_BASH_STRENGTH) {
      return;
    }

    if (ps.y < GOAL_DEFENCE_Y - 10) {
      y = -1;
    }
    else if (ps.y > GOAL_DEFENCE_Y + 10) {
      y = 1;
    }

    if (x < 0) { // ball on left
      if (x < FIELD_WIDTH/2 - GOAL_RANGE) {
        x = 1;
      }
      else {
        x = -1;
      }
    }
    if (x > 0) {
      if (x > FIELD_WIDTH/2 + GOAL_RANGE) {
        x = -1;
      }
      else {
        x = 1;
      }
    }
  };

  void run() {
    // run motors using motor controller
    mc.runMotors(x, y, tilt, offset);
  };

  bool defenceCheck() {
    if (teamDir == 0) {
      return false;
    }

    if (teamDir > 3 && teamDir < 9) {
      return false;
    }

    if (teamStr < 35) {
      return false;
    }

    if (direction == 0) {
      return false;
    }

    if (direction > 3 && direction < 9) {
      return false;
    }

    if (strength > teamStr - TEAM_STRENGTH_DIFFERANCE) {
      return false;
    }

    return true;
  };
};
