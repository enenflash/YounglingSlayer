#include <iostream>
using namespace std;

const int SWITCH_PIN = 30;

class Motor {
  public:
    int direction_pin;
    int pwm_pin;

    void run_motor(int percentage) {
      // determine whether to run clockwise or anti-clockwise
      if (percentage >= 0) {
        digitalWrite(direction_pin, HIGH);
      }

      else {
        digitalWrite(direction_pin, LOW);
      }

      // ensure values above 100 are converted to 100
      if (abs(percentage)>100) {
        if (percentage<0) {
          percentage = -100;
        }
        
        else {
          percentage = 100;
        }
      }


      // determine speed and return output value
      int speedValue = (100.0 - abs(percentage)) / 100.0 * 255.0;
      //int speedValue = map(actual_percentage, 0, 100, 0, 255);
      analogWrite(pwm_pin, speedValue);
    }
};

bool Switch_Idle() {
  int switchState = digitalRead(SWITCH_PIN);

  // return true when idle switch is off (as in the robot should be idle)
  if (switchState == LOW) {
    return true;
  }

  // return false when idle switch is on (as in the robot should run)
  else {
    return false;
  }
}

// SETUP
void setup() {
  // PWM Pins
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(8, OUTPUT);
  analogWriteFrequency(2, 20000);
  analogWriteFrequency(4, 20000);
  analogWriteFrequency(6, 20000);
  analogWriteFrequency(8, 20000);

  // direction pins
  pinMode(1, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(7, OUTPUT);

  // Set initial direction
  //digitalWrite(1, LOW); // Set to HIGH for CW, LOW for CCW
  //digitalWrite(3, LOW);
  //digitalWrite(5, LOW);
  //digitalWrite(7, LOW);

  // switch pin
  pinMode(SWITCH_PIN, INPUT_PULLDOWN);
}

// MAIN LOOP
void loop() {
  // ############# MOTOR OBJECTS #############
  Motor bl_motor;
  bl_motor.direction_pin = 1;
  bl_motor.pwm_pin = 2;

  Motor br_motor;
  br_motor.direction_pin = 3;
  br_motor.pwm_pin = 4;

  Motor tl_motor;
  tl_motor.direction_pin = 5;
  tl_motor.pwm_pin = 6;

  Motor tr_motor;
  tr_motor.direction_pin = 7;
  tr_motor.pwm_pin = 8;

  bool idle = Switch_Idle();

  while (idle) {
    tl_motor.run_motor(0);
    tr_motor.run_motor(0);
    bl_motor.run_motor(0);
    br_motor.run_motor(0);
    idle = Switch_Idle();
    digitalWrite(13, LOW);
  }


  // ############# WHILE LOOP #############
  while (not idle) {
    // run motors
    tl_motor.run_motor(100);
    tr_motor.run_motor(-100);
    bl_motor.run_motor(100);
    br_motor.run_motor(-100);
    idle = Switch_Idle();
    digitalWrite(13, HIGH);
  }
}
