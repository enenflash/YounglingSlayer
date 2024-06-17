#include <iostream>
#include <cmath>
using namespace std;

//theta and tilt both use unit circle format (and are in radians)
float getTheta(float x, float y) { // in radians
    //cout << "x: " << x << " y: " << y;
    float theta = atan(y / x);
    cout << "theta: " << theta;
    return theta;
}

int main() {
    getTheta(5, 4);
    return 0;
}
