// this code is designed to be run in pure c++ for debugging purposes
// to run in arduinoIDE remove all cout and remove stuff at lines 23, 194 and 244
// if M_PI works in arduinoIDE remove line 21

#include <iostream>
#include <algorithm>
#include <array>
#include <cmath>

using namespace std;

#define TL_PWM 1
#define TL_DIR 2
#define TR_PWM 3
#define TR_DIR 4
#define BL_PWM 5
#define BL_DIR 6
#define BR_PWM 7
#define BR_DIR 8

#define M_PI 3.1415926535

// remove this when running in arduinoIDE
#define HIGH true
#define LOW false
void digitalWrite(int dir_pin, bool freq) {
    return;
}
void analogWrite(int pwm_pin, float speed) {
    return;
}
// remove this ^^^^

//Motor controls individual motors on a hardware level
class Motor {
public:
	int PWM_PIN;
	int DIR_PIN;
	Motor(int pwm_pin, int dir_pin) {
		PWM_PIN = pwm_pin;
		DIR_PIN = dir_pin;
	};
	void run(float speedPercent) {
        float pwmSpeed = (100 - speedPercent) / 100 * 255;
        if (pwmSpeed < 0) {
            pwmSpeed = pwmSpeed * -1;
        }
        //pwm speed and clockwise derived
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
	}
};

//Motor controller interacts with 4 Motors
//MotorController(bool headlessMode)
class MotorController {
private: 
    bool headless;
    float speed=100.0;
    array<float, 4> motorSpeeds;

    Motor TL = Motor(TL_PWM, TL_DIR);
    Motor TR = Motor(TR_PWM, TR_DIR);
    Motor BL = Motor(BL_PWM, BL_DIR);
    Motor BR = Motor(BR_PWM, BR_DIR);

public:
    MotorController(bool headlessMode) {
        headless = headlessMode;
    };

private:
    //theta and tilt both use unit circle format (and are in radians)
    //getTheta(float x, float y) -> float theta
    float getTheta(float x, float y) { // in radians
        float theta = atan(y / x);
        if (theta < 0) theta = 2 * M_PI + theta;
        else if (theta == -0) theta = M_PI;
        if (x < 0 and y < 0) {
            theta += M_PI;
        }
        else if (x < 0 and y > 0) {
            theta -= M_PI;
        }
        return theta;
    }
    //getHeadlessTheta(float theta, float tilt) -> float headlessTheta
    float getHeadlessTheta(float theta, float tilt) {
        float headlessTheta = theta - tilt;
        return headlessTheta;

        //Headless theta is the true direction that the robot must go in no matter the tilt direction
    }

    //TL TR BL BR
    //getMotorSpeeds(float theta, float speed, float tilt) -> array<float, 4> motorSpeeds
    array<float, 4> getMotorSpeeds(float theta, float speed, float tilt) {
        //the effect that tilt has on the motors
        float t = tilt * 2;

        float x = cos(theta);
        float y = sin(theta);

        float motorRatio[4] = { -x - y + t, -x + y + t, x - y + t, x + y + t };
        // round motor Ratio to prevent errors
        for (int i = 0; i < 4; i++) {
            motorRatio[i] = round(motorRatio[i] * 1000) / 1000;
        }

        float highestVal = *max_element(motorRatio, motorRatio + 4);
        float lowestVal = *min_element(motorRatio, motorRatio + 4);

        float largest;
        if (highestVal > lowestVal * -1) {
            largest = highestVal;
        }
        else {
            largest = lowestVal * -1;
        }

        array<float, 4> motorSpeeds;

        //avoid division by 0
        if (largest == 0) {
            motorSpeeds = { 0, 0, 0, 0 };
            return motorSpeeds;
        }

        for (int i = 0; i < 4; i++) {
            // +0.0 converts motorRatio to double
            motorSpeeds[i] = (motorRatio[i] + 0.0) / largest * speed;
        }

        //TL, TR, BL, BR
        return motorSpeeds;
    };

public:
    array<float, 4> getMotorSpeeds() {
        return motorSpeeds;
    };
    void setHeadless(bool headlessMode) {
        headless = headlessMode;
    };
    void setSpeed(float newSpeed) {
        speed = newSpeed;
    };
    void accelerate(float step, float target) {
        speed += step;
        if (speed > target) {
            speed = target;
        }
    };
    void deaccelerate(float step, float target) {
        speed -= step;
        if (speed < target) {
            speed = target;
        }
    };
    void runMotors(float x = 0, float y = 0, float tilt = 0) {
        if (x == 0 and y == 0) {
            stopMotors();
            return;
        }
        float theta = getTheta(x, y);
        if (headless) {
            theta = getHeadlessTheta(theta, tilt);
        }
        motorSpeeds = getMotorSpeeds(theta, speed, tilt);

        TL.run(motorSpeeds[0]);
        TR.run(motorSpeeds[1]);
        BL.run(motorSpeeds[2]);
        BR.run(motorSpeeds[3]);
    };
    void stopMotors() {
        motorSpeeds = { 0, 0, 0, 0 };
        TL.stop();
        TR.stop();
        BL.stop();
        BR.stop();
    };
};

// remove this function (this is for testing motorSpeeds)
void addMotorVectors(array<float, 4> motorSpeeds) {
    float totalI = 0, totalJ = 0;

    if (motorSpeeds[0] != 0) {
        totalI += -(motorSpeeds[0] / abs(motorSpeeds[0])) * cos(M_PI / 4) * abs(motorSpeeds[0]);
        totalJ += -(motorSpeeds[0] / abs(motorSpeeds[0])) * sin(M_PI / 4) * abs(motorSpeeds[0]);
    }

    if (motorSpeeds[1] != 0) {
        totalI += -(motorSpeeds[1] / abs(motorSpeeds[1])) * cos(M_PI / 4) * abs(motorSpeeds[1]);
        totalJ += (motorSpeeds[1] / abs(motorSpeeds[1])) * sin(M_PI / 4) * abs(motorSpeeds[1]);
    }

    if (motorSpeeds[2] != 0) {
        totalI += (motorSpeeds[2] / abs(motorSpeeds[2])) * cos(M_PI / 4) * abs(motorSpeeds[2]);
        totalJ += -(motorSpeeds[2] / abs(motorSpeeds[2])) * sin(M_PI / 4) * abs(motorSpeeds[2]);
    }

    if (motorSpeeds[3] != 0) {
        totalI += (motorSpeeds[3] / abs(motorSpeeds[3])) * cos(M_PI / 4) * abs(motorSpeeds[3]);
        totalJ += (motorSpeeds[3] / abs(motorSpeeds[3])) * sin(M_PI / 4) * abs(motorSpeeds[3]);
    }

    float speed = sqrt(totalI*totalI + totalJ*totalJ)/4;
    float bearing = atan(totalJ / totalI);

    if (bearing < 0) bearing = 2 * M_PI + bearing;
    else if (bearing == -0) bearing = M_PI;
    if (totalI < 0 and totalJ < 0) {
        bearing += M_PI;
    }
    else if (totalI < 0 and totalJ > 0) {
        bearing -= M_PI;
    }

    cout << "\nSpeed: " << speed << " Bearing: " << bearing*180/M_PI;
};

int main() {
    MotorController motorController(true);
    motorController.runMotors(0, 0, 0);

    array<float, 4> motorSpeeds = motorController.getMotorSpeeds();

    cout << "\nMotor Speeds:";
    for (int i = 0; i < 4; i++) {
        cout << " " << motorSpeeds[i];
    }

    addMotorVectors(motorSpeeds);
    motorController.stopMotors();

    cout << "\n";

	return 0;
}
