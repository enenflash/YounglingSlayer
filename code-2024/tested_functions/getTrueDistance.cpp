#include <iostream>
#include <cmath>
using namespace std;

//tilt uses unit circle format
float getTrueDistance(float distance, float tilt) {
    if (tilt == 0) {
        return distance;
    }
    else if (tilt < 0) {
        tilt = tilt*-1;
    }
    
    float trueDistance = cos(tilt) * distance;
    cout << "True Distance: " << trueDistance;
    return trueDistance;
}

int main() {
    float trueDistance = getTrueDistance(50, 0.3);
    return 0;
}
