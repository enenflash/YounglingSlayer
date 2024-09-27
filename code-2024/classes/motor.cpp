#include <iostream>
using namespace std;

class Motor {
    public: 
    int DIR_PIN;
    int PWM_PIN;
    Motor(int dir_pin, int pwm_pin) {
        DIR_PIN = dir_pin;
        PWM_PIN = pwm_pin;
    }
    void run(float speedPercent) {
        float pwmSpeed = (100-speedPercent)/100*255;
        if (pwmSpeed < 0) {
            pwmSpeed = pwmSpeed * -1;
        }
        //pwm speed and clockwise derived
        //WARNING UNCHECKED CODE
        if (speedPercent > 0) {
            digitalWrite(DIR_PIN, HIGH);
        }
        else {
            digitalWrite(DIR_PIN, LOW);
        }
        
        analogWrite(PWM_PIN, pwmSpeed);
        
    };
    void stop() {
        analogWrite(PWM_PIN, 255);
    };
};

int main() {
    Motor TL(2, 3);
    TL.run(50);
    TL.stop();
    return 0;
}
