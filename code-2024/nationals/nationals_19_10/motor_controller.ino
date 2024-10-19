//Motor controls individual motors on a hardware level
class Motor {
public:
	int PWM_PIN;
	int DIR_PIN;
	Motor(int pwm_pin, int dir_pin) {
		PWM_PIN = pwm_pin;
		DIR_PIN = dir_pin;
	};
  // run the motor
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
  // stop the motor
	void stop() {
    analogWrite(PWM_PIN, 255);
	};
};

//Motor controller interacts with 4 Motors
//MotorController(bool headlessMode)
class MotorController {
private:
  // variable initilisation
  bool headless;
  float speed = 100.0;
  array<float, 4> motorSpeeds;

  // motor objects
  Motor TL = Motor(TL_PWM, TL_DIR);
  Motor TR = Motor(TR_PWM, TR_DIR);
  Motor BL = Motor(BL_PWM, BL_DIR);
  Motor BR = Motor(BR_PWM, BR_DIR);

public:
  MotorController(bool headlessMode) {
    headless = headlessMode;
  };

private:
  //TL TR BL BR
  //getMotorSpeeds(float theta, float speed, float tilt) -> array<float, 4> motorSpeeds
  array<float, 4> getMotorSpeeds(float x, float y, float speed, float tilt, float offset) {
    //the effect that tilt has on the motors
    //convert to bearing
    tilt = 360 - tilt;
    if (tilt > 180) {
      tilt = -(360 - tilt);
    }

    if (tilt < TILT_RANGE and tilt > -TILT_RANGE) {
      tilt = 0;
    }
    
    // float t = 7.45*pow(abs(tilt), (1.0/2.0));
    // if (tilt < 0) {
    //   t = t*-1;
    // }

    // float o = 7.45*pow(abs(offset), (1.0/3.0));
    // if (offset < 0) {
    //   o = o*-1;
    // }

    Serial.print("Offset: ");
    Serial.println(offset);

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
      motorSpeeds = { tilt + offset, tilt + offset, tilt + offset, tilt + offset };
      return motorSpeeds;
    }

    for (int i = 0; i < 4; i++) {
      // +0.0 converts motorRatio to double
      motorSpeeds[i] = ((motorRatio[i] + 0.0) / largest * speed) + tilt + offset;
    }

    //TL, TR, BL, BR
    return motorSpeeds;
  };

public:
  array<float, 4> getMotorSpeeds() {
    return motorSpeeds;
  };
  float getSpeed() {
    return speed;
  };

  void setHeadless(bool headlessMode) {
    headless = headlessMode;
  };

  // sets speed in percentage
  void setSpeed(float newSpeed) {
    speed = newSpeed;
  };

  // acceleration and deacceleration functions (just change the speed)
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

  // run motors using x and y coordinates (and tilt for gyro correction)
  void runMotors(float initial_x = 0, float initial_y = 0, float tilt = 0, float offset = 0) {
    float x = 0, y = 0;
    if (headless) {
      x = initial_x*cos(-tilt) - initial_y*sin(-tilt);
      y = initial_x*sin(-tilt) + initial_y*cos(-tilt);
    }
    else {
      x = initial_x, y = initial_y;
    }
      
    motorSpeeds = getMotorSpeeds(x, y, speed, tilt, offset);

    for (int i = 0; i < 4; i++) {
      if (motorSpeeds[i] > 100) {
        motorSpeeds[i] = 100;
      }
      else if (motorSpeeds[i] < -100) {
        motorSpeeds[i] = -100;
      }
    }

    // run motors
    TL.run(motorSpeeds[0]);
    TR.run(motorSpeeds[1]);
    BL.run(motorSpeeds[2]);
    BR.run(motorSpeeds[3]);
  };

  // run using raw motor speeds in percentage
  void runRaw(float tlSpeed, float trSpeed, float blSpeed, float brSpeed) {
    TL.run(tlSpeed);
    TR.run(trSpeed);
    BL.run(blSpeed);
    BR.run(brSpeed);
  };

  // stop all motors
  void stopMotors() {
    motorSpeeds = { 0, 0, 0, 0 };
    TL.stop();
    TR.stop();
    BL.stop();
    BR.stop();
  };
};