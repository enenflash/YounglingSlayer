//Motor controls individual motors on a hardware level
class Motor {
public:
	int PWM_PIN;
	int DIR_PIN;
	Motor(int pwm_pin, int dir_pin) {
		PWM_PIN = pwm_pin;
		DIR_PIN = dir_pin;
	};
	void run(float speedPercent) {
    int pwmSpeed = (100 - abs(speedPercent)) / 100 * 255;
    //pwm speed and clockwise derived
    if (speedPercent > 0) {
      digitalWrite(DIR_PIN, LOW);
    }
    else {
      digitalWrite(DIR_PIN, HIGH);
    }

    analogWrite(PWM_PIN, pwmSpeed);
	};
	void stop() {
    analogWrite(PWM_PIN, 255);
	};
};

//Motor controller interacts with 4 Motors
//MotorController(bool headlessMode)
class MotorController {
private: 
  bool headless;
  float speed=100.0;
  array<float, 4> motorSpeeds;

  Motor TL = Motor(TL_PWM, TL_DIR);
  Motor TR = Motor(TR_PWM, TR_DIR);
  Motor BL = Motor(BL_PWM, BL_DIR);
  Motor BR = Motor(BR_PWM, BR_DIR);

public:
  MotorController(bool headlessMode) {
    headless = headlessMode;
  };

private:
  //theta and tilt both use unit circle format (and are in radians)
  //getTheta(float x, float y) -> float theta
  float getTheta(float x, float y) { // in radians
    float theta = atan(y / x);
    if (theta < 0) theta = 2 * M_PI + theta;
    else if (theta == -0) theta = M_PI;
    if (x < 0 and y < 0) {
      theta += M_PI;
    }
    else if (x < 0 and y > 0) {
      theta -= M_PI;
    }
    return theta;
  }
  //getHeadlessTheta(float theta, float tilt) -> float headlessTheta
  float getHeadlessTheta(float theta, float tilt) {
    if (isnan(theta)) {
      return theta;
    }
    if (tilt < 10 or tilt > 350) {
      return theta;
    }
    // convert to radians
    tilt = tilt * M_PI / 180.0;
    float headlessTheta = theta - tilt;

    if (headlessTheta < 0) {
      headlessTheta = 2*M_PI + headlessTheta;
    }

    return headlessTheta;

    //Headless theta is the true direction that the robot must go in no matter the tilt direction
  }

  //TL TR BL BR
  //getMotorSpeeds(float theta, float speed, float tilt) -> array<float, 4> motorSpeeds
  array<float, 4> getMotorSpeeds(float theta, float speed, float tilt) {
    //the effect that tilt has on the motors
    //convert to bearing
    tilt = 360 - tilt;
    if (tilt > 180) {
      tilt = -(360 - tilt);
    }

    if (tilt < 5 and tilt > -5) {
      tilt = 0;
    }

    float t = 7.45*pow(abs(tilt), (1.0/2.0));
    if (tilt < 0) {
      t = t*-1;
    }

    float x, y;

    if (isnan(theta)) {
      x = 0;
      y = 0;
    }
    else {
      x = cos(theta);
      y = sin(theta);
    }

    float motorRatio[4] = { -x - y, -x + y, x - y, x + y };
    // round motor Ratio to prevent errors
    for (int i = 0; i < 4; i++) {
      motorRatio[i] = round(motorRatio[i] * 1000) / 1000;
    }

    float highestVal = *max_element(motorRatio, motorRatio + 4);
    float lowestVal = *min_element(motorRatio, motorRatio + 4);

    float largest;
    if (highestVal > lowestVal * -1) {
      largest = highestVal;
    }
    else {
      largest = lowestVal * -1;
    }

    array<float, 4> motorSpeeds;

    //avoid division by 0
    if (largest == 0) {
      motorSpeeds = { t, t, t, t };
      return motorSpeeds;
    }

    for (int i = 0; i < 4; i++) {
      // +0.0 converts motorRatio to double
      motorSpeeds[i] = ((motorRatio[i] + 0.0) / largest * speed) + t;
    }

    //TL, TR, BL, BR
    return motorSpeeds;
  };

public:
  array<float, 4> getMotorSpeeds() {
    return motorSpeeds;
  };
  void setHeadless(bool headlessMode) {
    headless = headlessMode;
  };
  void setSpeed(float newSpeed) {
    speed = newSpeed;
  };
  void accelerate(float step, float target) {
    speed += step;
    if (speed > target) {
      speed = target;
    }
  };
  void deaccelerate(float step, float target) {
    speed -= step;
    if (speed < target) {
      speed = target;
    }
  };
  void runMotors(float x = 0, float y = 0, float tilt = 0) {

    float theta = getTheta(x, y);
    if (headless) {
      theta = getHeadlessTheta(theta, tilt);
    }

    motorSpeeds = getMotorSpeeds(theta, speed, tilt);

    for (int i = 0; i < 4; i++) {
      if (motorSpeeds[i] > 100) {
        motorSpeeds[i] = 100;
      }
      else if (motorSpeeds[i] < -100) {
        motorSpeeds[i] = -100;
      }
    }

    TL.run(motorSpeeds[0]);
    TR.run(motorSpeeds[1]);
    BL.run(motorSpeeds[2]);
    BR.run(motorSpeeds[3]);
  };

  void runRaw(float tlSpeed, float trSpeed, float blSpeed, float brSpeed) {
    TL.run(tlSpeed);
    TR.run(trSpeed);
    BL.run(blSpeed);
    BR.run(brSpeed);
  };

  void stopMotors() {
    motorSpeeds = { 0, 0, 0, 0 };
    TL.stop();
    TR.stop();
    BL.stop();
    BR.stop();
  };
};
