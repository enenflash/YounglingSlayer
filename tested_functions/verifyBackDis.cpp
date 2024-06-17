#include <iostream>

using namespace std;

//account for goal affecting backDis
float verifyBackDis(float leftDis, float backDis) {
    if (leftDis > 68.5 and leftDis < 113.5) {
        backDis += 22.6;
    }
    cout << "\nBackDis: " << backDis;
    return backDis;
}

int main() {
    float backDis = 54;
    backDis = verifyBackDis(91, backDis);
    return 0;
}
