#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>

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

#define SWITCH_PIN 30
