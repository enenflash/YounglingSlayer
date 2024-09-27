#include <iostream>
using namespace std;

#define DF_PWM 10
#define DF_DIR1 35
#define DF_DIR2 36
#define TOGGLE_PIN 40

class Dribbler_Motor {
public:
	int PWM_PIN;
	int DIR1_PIN;
  int DIR2_PIN;
	Dribbler_Motor(int pwm_pin, int dir1_pin, int dir2_pin) {
		PWM_PIN = pwm_pin;
		DIR1_PIN = dir1_pin;
    DIR2_PIN = dir2_pin;
	};

	void run(float speedPercent) {
    int pwmSpeed = (100 - abs(speedPercent)) / 100 * 255;
    //pwm speed and clockwise derived
    if (speedPercent > 0) {
      digitalWrite(DIR1_PIN, LOW);
      digitalWrite(DIR2_PIN, HIGH);
    }
    else if (speedPercent < 0) {
      digitalWrite(DIR1_PIN, HIGH);
      digitalWrite(DIR2_PIN, LOW);
    }

    else {
      digitalWrite(DIR1_PIN, LOW);
      digitalWrite(DIR2_PIN, LOW);
    }

    analogWrite(PWM_PIN, pwmSpeed);
	};
	void stop() {
    analogWrite(PWM_PIN, 255);
	};
};

Dribbler_Motor DF = Dribbler_Motor(DF_PWM, DF_DIR1, DF_DIR2);

void setup() {
  Serial.begin(9600);
  pinMode(DF_PWM, OUTPUT);
  pinMode(DF_DIR1, OUTPUT);
  pinMode(DF_DIR2, OUTPUT);
  pinMode(TOGGLE_PIN, INPUT_PULLDOWN);
}

void loop() {
  int switch_state = digitalRead(TOGGLE_PIN);

  if (switch_state == HIGH) {
    DF.run(-100);
  }

  else {
    DF.run(0);
  }
  
  delay(100);

}
