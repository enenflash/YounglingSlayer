#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_map>

using namespace std;

unordered_map<int, float> IR_angles = {
    {3, 0.0}, {2, M_PI/6}, {1, M_PI/3},
    {12, M_PI/2}, {11, 2*M_PI/3}, {10, 5*M_PI/6},
    {9, M_PI}, {8, 7*M_PI/6}, {7, 4*M_PI/3},
    {6, 3*M_PI/2}, {5, 5*M_PI/3}, {4, 11*M_PI/6},
    {0, M_PI/2}
};

//gets ball vector from direction and ballDis
vector<float> getBallVector(int direction, float ballDis) {
    float ballAngle = IR_angles[direction];
    cout << "\nBall Angle: " << ballAngle;
    
    float ballX = round(cos(ballAngle)) * ballDis;
    float ballY = round(sin(ballAngle)) * ballDis;
    
    cout << "\nBall x: " << ballX << " Ball y: " << ballY;
    
    vector<float> ballVector = {ballX, ballY};
    return ballVector;
}

int main() {
    vector<float> ballVector = getBallVector(12, 10);
    return 0;
}
