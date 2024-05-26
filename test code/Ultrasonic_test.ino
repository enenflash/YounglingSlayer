#include <Adafruit_BNO055.h>

#include <iostream>
using namespace std;

class Ultrasonic {
  public:
    int trigger_pin;
    int echo_pin;

    int16_t get_distance() {
      long duration, distance;

      // clear trigger pin
      digitalWrite(trigger_pin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigger_pin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigger_pin, LOW);

      // get duration in miliseconds
      duration = pulseIn(echo_pin, HIGH);

      // converet to cm
      distance = (duration / 29) / 2;  // Calculating the distance

      return distance;
    }
};

void setup() {
  pinMode(5, OUTPUT); // Sets the triggerPin as an Output
  pinMode(10, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
}

void loop() {
  Ultrasonic left_ultrasonic;
  left_ultrasonic.echo_pin = 10;
  left_ultrasonic.trigger_pin = 5;
  int16_t distance = left_ultrasonic.get_distance();
  
  Serial.print("Distance: "); // Prints the distance on the Serial Monitor
  Serial.println(distance);
  
  delay(100); // Waiting 100ms second
  digitalWrite(13, HIGH);
}
