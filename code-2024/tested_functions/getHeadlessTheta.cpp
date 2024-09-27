#include <iostream>
using namespace std;

//theta and tilt both use unit circle format (and are in radians)
float getHeadlessTheta(float theta, float tilt) {
    //cout << "theta: " << theta << " tilt: " << tilt;
    
    float headlessTheta = theta - tilt;
    cout << "\nHeadlessTheta: " << headlessTheta;
    return headlessTheta;
    
    //Headless theta is the true direction that the robot must go in no matter the tilt direction
}

int main() {
    float headlessTheta = getHeadlessTheta(0.33, 0.55);
    cout << "\nHeadlessTheta: " << headlessTheta;
    return 0;
}
