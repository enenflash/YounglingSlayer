#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>
#include <numeric> // used for calculating sum of vectors


// For IMU
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

using namespace std;

#define TL_PWM 11
#define TL_DIR 12

#define TR_PWM 1
#define TR_DIR 2

#define BL_PWM 7
#define BL_DIR 8

#define BR_PWM 3
#define BR_DIR 4

#define SWITCH_PIN 36

#define MIN_BALL_DISTANCE 65 // any less than 65 and it counts as close

// ultrasonic pins
#define LU_TRIG 35
#define LU_ECHO 34
#define RU_TRIG 41
#define RU_ECHO 40
#define BU_TRIG 21
#define BU_ECHO 20


