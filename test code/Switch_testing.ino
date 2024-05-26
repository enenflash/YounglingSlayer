#include <iostream>
using namespace std;
const int switchPin = 10; // Change this to the actual pin you're using

bool Switch_Idle(){
  int switchState = digitalRead(30);

  // return true when idle switch is off (as in the robot should be idle)
  if (switchState == LOW) {
    return true;
  }
  
  // return false when idle switch is on (as in the robot should run)
  else {
    return false;
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(switchPin, INPUT_PULLDOWN); // Changed to INPUT_PULLUP
  // Additional setup code if needed
}

void loop() {
  bool idle = Switch_Idle();
  while (idle) {
    Serial.print("IDLE")
    Serial.println("")
    idle = Switch_Idle();
  }

  while (not idle) {
    Serial.print("RUNNING")
    Serial.println("")
    idle = Switch_Idle();
  }
  digitalWrite(13, HIGH);
  
  delay(250);

  // Add any other code you need to run continuously
}
