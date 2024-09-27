#include <iostream>
using namespace std;
void setup() {
  pinMode(2, OUTPUT);
  pinMode(1, OUTPUT);
  
  // Set initial direction
  digitalWrite(1, HIGH); // Set to HIGH for CW, LOW for CCW
}

void loop() {
  int direction_pin = 1;
  int pwm_pin = 2;
  float percentage = 100;
  //digitalWrite(direction_pin, HIGH);
  
  int speedValue = (int)((100-percentage) / 100.0 * 255.0);
  //speedValue = 255 - speedValue;
  analogWrite(pwm_pin, speedValue);

  digitalWrite(13, HIGH);
}
