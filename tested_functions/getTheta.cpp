#include <iostream>
#include <cmath>
using namespace std;

//theta and tilt both use unit circle format (and are in radians)
float getTheta(float x, float y) { // in radians
    float theta = atan(y / x);
    if (theta < 0) theta = 2*M_PI+theta;
    else if (theta == -0) theta = M_PI;
    if (x < 0 and y < 0) {
        theta += M_PI;
    }
    else if (x < 0 and y > 0) {
        theta -= M_PI;
    }
    cout << theta*180/M_PI;
    return theta;
}

int main() {
    getTheta(5, 4);
    return 0;
}
