#include <iostream>
using namespace std;

class Ultrasonic {
    public:
    int TRIG_PIN;
    int ECHO_PIN;
    Ultrasonic(int trig_pin, int echo_pin) {
        TRIG_PIN = trig_pin;
        ECHO_PIN = echo_pin;
    }
    int readDistance() {
        long duration, distance;

        // clear trigger pin
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);

        // get duration in miliseconds
        duration = pulseIn(ECHO_PIN, HIGH);

        // converet to cm
        distance = (duration / 29) / 2;  // Calculating the distance

        return distance;
    };
};

int main() {
    Ultrasonic US_L(2, 3);
    US_L.readDistance();
    return 0;
}
