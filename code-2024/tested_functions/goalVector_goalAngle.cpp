#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

//all values in cm
const int fieldDim[2] = {182, 243};
const int goalDispFromBL[2] = {91, 213};

vector<float> getGoalVector(float leftDist, float backDist) {
    
    vector<float> goalVector = {
        goalDispFromBL[0] - leftDist,
        goalDispFromBL[1] - backDist
    };
    
    cout << "goalX: " << goalVector[0];
    cout << "\ngoalY: " << goalVector[1];
    
    return goalVector;
}

float getGoalAngle(vector<float> goalVector) {
    float goalAngle = atan(goalVector[1]/goalVector[0]);
    cout << "\ngoalAngle: " << goalAngle;
    return goalAngle;
}

int main() {
    float leftDis = 91, backDis = 91.5;
    vector<float> goalVector = getGoalVector(leftDis, backDis);
    float goalAngle = getGoalAngle(goalVector);
    return 0;
}
