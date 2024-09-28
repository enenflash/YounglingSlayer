// use in ino file
// uses BNO, IR and motor controller classes
// (it also reads from the line sensor internally)

class Robot {
public:
  MotorController mc = MotorController(false);
  Adafruit_BNO055 bno = Adafruit_BNO055(55);
  IRSensor ir;

  float tilt = 0;
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
      lineValue = Serial6.read();
    }
  };

  void getXY(float &x, float &y, int direction, int ballDist) {
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

    if (y < 0) {
      if (x > 0) {
        x -= ROBOT_TO_BALL;
      }
      else {
        x += ROBOT_TO_BALL;
      }
    }
    else {
      y -= ROBOT_TO_BALL;
    }
  };

public:
  void update() {
    // get sensor data using sensor classes
    getSensorData();
  };

  bool ballBehind() {
    return (PI < irAngles[direction] && irAngles[direction] < 2*PI);
  };

  void getBehindBall() {
    // get XY based on ball position
    getXY(x, y, direction, ballDist);
    
    // run motors using motor controller
    mc.runMotors(x, y, tilt);
  };
};
