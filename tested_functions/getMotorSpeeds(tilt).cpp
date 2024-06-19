// THIS FUNCTION GETS MOTOR SPEEDS FROM AN ANGLE
// The angle uses unit circle format

#include <iostream>
#include <algorithm> //*max_element, *min_element
#include <array>
#include <cmath>
using namespace std;

array<float, 4> getMotorSpeeds(float theta, float speed, float tilt) {
    //cout << theta << "\n" << speed;
    
    //the effect that tilt has on the motors
    float t = tilt*2;
    
    float x = cos(theta);
    float y = sin(theta);
    
    cout << "\nx: " << x << " y: " << y;
    
    float motorRatio[4] = {-x-y+t, -x+y+t, x-y+t, x+y+t};
    // round motor ratio to prevent errors
    for (int i = 0; i < 4; i++) {
        motorRatio[i] = round(motorRatio[i] * 1000) / 1000;
    }
    
    cout << "\n\nMotor Ratio: \n";
    for (int i = 0; i < 4; i++) {
        cout << " " << motorRatio[i];
    }
    
    float highestVal = *max_element(motorRatio, motorRatio+4);
    float lowestVal = *min_element(motorRatio, motorRatio+4);
    cout << "\n\nHighest Value: " << highestVal;
    cout << "\nLowest Value: " << lowestVal;
    
    float largest;
    if (highestVal > lowestVal*-1) {
        largest = highestVal;
    }
    else {
        largest = lowestVal*-1;
    }
    cout << "\nLargest: " << largest;
    
    array<float, 4> motorSpeeds;
    
    //avoid division by 0
    if (largest == 0) {
        motorSpeeds = {0, 0, 0, 0};
        return motorSpeeds;
    }
    
    cout << "\n\nMotor Speed Values: \n";
    for (int i = 0; i < 4; i++) {
        // +0.0 converts motorRatio to double
        motorSpeeds[i] = (motorRatio[i] + 0.0) / largest * speed;
        cout << " " << motorSpeeds[i];
    }
    
    //TL, TR, BL, BR
    return motorSpeeds;
}

int main() {
    array<float, 4> motorSpeeds = getMotorSpeeds(M_PI/6, 100, 0);
    return 0;
}
