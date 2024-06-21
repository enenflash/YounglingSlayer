// This test code moves the robot in a specific direction while correcting tilt

#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>

// For IMU
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <Wire.h>

using namespace std;

#define TL_PWM 11
#define TL_DIR 12

#define TR_PWM 1
#define TR_DIR 2

#define BL_PWM 7
#define BL_DIR 8

#define BR_PWM 3
#define BR_DIR 4

#define SWITCH_PIN 30

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
    // convert to radians
    tilt = tilt * M_PI * 180;
    float headlessTheta = theta - tilt;
    return headlessTheta;

    //Headless theta is the true direction that the robot must go in no matter the tilt direction
  }

  //TL TR BL BR
  //getMotorSpeeds(float theta, float speed, float tilt) -> array<float, 4> motorSpeeds
  array<float, 4> getMotorSpeeds(float theta, float speed, float tilt) {
    //the effect that tilt has on the motors
    //convert to bearing
    float t = 360 - tilt;
    if (t > 180) {
      t = -(360 - t);
    }

    float x = cos(theta);
    float y = sin(theta);

    t = t/16;

    float motorRatio[4] = { -x - y + t, -x + y + t, x - y + t, x + y + t };
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
      motorSpeeds = { 0, 0, 0, 0 };
      return motorSpeeds;
    }

    for (int i = 0; i < 4; i++) {
      // +0.0 converts motorRatio to double
      motorSpeeds[i] = (motorRatio[i] + 0.0) / largest * speed;
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
    if (x == 0 and y == 0) {
      stopMotors();
      return;
    }
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

// class IMU {
// public:
//   float heading() {
//     sensors_event_t event;
//     bno.getEvent(&event);
//     return event.orientation.x;
//   };
//   float pitch() {
//     sensors_event_t event;
//     bno.getEvent(&event);
//     return event.orientation.y;
//   };
//   float roll() {
//     sensors_event_t event;
//     bno.getEvent(&event);
//     return event.orientation.z;
//   };
//   // void reset() {
//   //   // WARNING UNTESTED
//   //   // connect reset pin of gyro to any 5v GPIO pin
//   //   // send the pin low then high to reset
//   //   Serial.println("Resetting.");
//   //   digitalWrite(RESET_PIN, LOW);
//   //   delayMicroseconds(30);
//   //   digitalWrite(RESET_PIN, HIGH);

//   //   bno.begin();
//   // };
// };

bool getIdle() {
  int switchState = digitalRead(SWITCH_PIN);
  // return true when idle switch is off (as in the robot should be idle)
  // return false when idle switch is on (as in the robot should run)
  return (switchState == LOW);
};

//IMU gyro;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

MotorController mc = MotorController(false);
float tilt;

void setup() {
  Serial.begin(9600);
  Serial.println("\nA");
  /* :::::::: MOTOR PINS :::::::: */

  pinMode(TL_PWM, OUTPUT);
  pinMode(TR_PWM, OUTPUT);
  pinMode(BL_PWM, OUTPUT);
  pinMode(BR_PWM, OUTPUT);

  analogWriteFrequency(TL_PWM, 20000);
  analogWriteFrequency(TR_PWM, 20000);
  analogWriteFrequency(BL_PWM, 20000);
  analogWriteFrequency(BR_PWM, 20000);

  pinMode(TL_DIR, OUTPUT);
  pinMode(TR_DIR, OUTPUT);
  pinMode(BL_DIR, OUTPUT);
  pinMode(BR_DIR, OUTPUT);

  pinMode(SWITCH_PIN, INPUT);

  mc.stopMotors();

  if (!bno.begin()) {
    Serial.print("\nNo BNO055 detected");

  }
  else { Serial.print("\nBNO055 Initialized"); }

  bno.setExtCrystalUse(true);
}

void loop() {
  sensors_event_t event;
  bno.getEvent(&event);
  tilt = event.orientation.x;

  tilt = 360 - tilt;

  if (tilt < 5 or tilt > 355) {
    tilt = 0;
  }

  if (!getIdle()) {
    mc.setSpeed(50);
    mc.runMotors(0, 1, tilt);
  }
  else {
    mc.stopMotors();
  }

  delay(100);
}
