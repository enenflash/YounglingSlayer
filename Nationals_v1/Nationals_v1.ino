#include <iostream>
#include <array> // to import imutable arrays with set size
#include <cmath> // for trig
#include <algorithm> 
#include <map> // dictionaries
#include <vector> // mutable lists with no set size
#include <numeric> // used for calculating sum of vectors


// For IMU
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

using namespace std;

// motor pins    // pins on PCB
#define BL_PWM 7 // 4
#define BL_DIR 8 // 3

#define TL_PWM 11 // 6
#define TL_DIR 12 // 5

#define TR_PWM 1 // 7 
#define TR_DIR 2 // 8 

#define BR_PWM 3 // 10
#define BR_DIR 4 // 9

// ultrasonic pins
#define LU_TRIG 35 // 22
#define LU_ECHO 34 // 23

#define RU_TRIG 41 // 34
#define RU_ECHO 40 // 33

#define BU_TRIG 21 // 28
#define BU_ECHO 20 // 27

// dribbler motors
#define DF_PWM 10
#define DF_DIR1 35
#define DF_DIR2 36
#define DB_PWM 11
#define DB_DIR1 37
#define DB_DIR2 38

// other pins
#define SWITCH_PIN 36 // 40

// other variables
#define MIN_BALL_DISTANCE 65 // any less than 65 and it counts as close
#define MAX_SPEED 100




