#include <iostream>
#include <algorithm> //*max_element, *min_element
#include <array>
using namespace std;

array<float, 4> getMotorSpeeds(int x, int y, float speed) {
    //cout << x << "\n" << y << "\n" << speed;
    
    int motorRatio[4] = {-x-y, -x+y, x-y, x+y};
    cout << "\nMotor Ratio: \n";
    for (int i = 0; i < 4; i++) {
        cout << " " << motorRatio[i];
    }
    
    int highestVal = *max_element(motorRatio, motorRatio+4);
    int lowestVal = *min_element(motorRatio, motorRatio+4);
    cout << "\n\nHighest Value: " << highestVal;
    cout << "\nLowest Value: " << lowestVal;
    
    int largest;
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
    array<float, 4> motorSpeeds = getMotorSpeeds(0, 1, 100);
    return 0;
}
