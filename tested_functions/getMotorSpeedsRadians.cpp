// THIS FUNCTION GETS MOTOR SPEEDS FROM AN ANGLE

#include <iostream>
#include <algorithm> //*max_element, *min_element
#include <array>
#include <cmath>
using namespace std;

array<float, 4> getMotorSpeeds(float theta, float speed) {
    //cout << theta << "\n" << speed;
    
    float x = cos(theta);
    float y = sin(theta);
    
    cout << "\nx: " << x << " y: " << y;
    
    float motorRatio[4] = {-x-y, -x+y, x-y, x+y};
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
    array<float, 4> motorSpeeds = getMotorSpeeds(M_PI/6, 100);
    return 0;
}
