// Example code for usage of functions
// WARNING: this code is outdated - it purely exists as a demo
// copy the functions from their individual cpp files

#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <cmath>

using namespace std;

float getTheta(float x, float y) { // in radians
    float theta = atan(y / x);
    cout << "theta: " << theta;
    return theta;
}

float getHeadlessTheta(float theta, float tilt) {
    
    float headlessTheta = theta - tilt;
    cout << "\nHeadlessTheta: " << headlessTheta;
    return headlessTheta;
    
    //Headless theta is the true direction that the robot must go in no matter the tilt direction
}

array<float, 4> getMotorSpeeds(float theta, float speed) {
    
    float x = cos(theta);
    float y = sin(theta);
    
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
    // raw x, y and tilt values evaluated previously
    float x = 3, y = 5, tilt = 0.1;
    float theta = getTheta(x, y);
    //headless theta allows the robot move in a "true" direction
    float headlessTheta = getHeadlessTheta(theta, tilt);
    array<float, 4> motorSpeeds = getMotorSpeeds(headlessTheta, 100);
    return 0;
}
