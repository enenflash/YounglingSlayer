#include <iostream>
#include <array>
#include <map>
#include <algorithm>
using namespace std;

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// Motor Pins
#define TL_PWM 5
#define TL_DIR 4
#define TR_PWM 6
#define TR_DIR 7
#define BL_PWM 3
#define BL_DIR 2
#define BR_PWM 9
#define BR_DIR 8

// Ultrasonic Pins
#define USR_TRIG 34
#define USR_ECHO 33
#define USL_TRIG 22
#define USL_ECHO 23
#define USB_TRIG 28
#define USB_ECHO 27

// IDLE SWITCH
#define IDLE_PIN 40

#define FIELD_WIDTH 182

const int DIR_HIST_LENGTH = 5;
const int STR_HIST_LENGTH = 5;

const int ULT_HIST_LENGTH = 5;

const int ROBOT_TO_BALL_SIDE = 9;
const int ROBOT_TO_BALL_BACK = 9;
const int ULTRASONIC_TO_ROBOT = 10;

// IR direction to radians
std::map<int, int> irAngles = {
  { 0,  0        },
  { 11, PI/3     },
  { 10, PI/6     },
  { 9,  0        },
  { 8,  11*PI/6  },
  { 7,  5*PI/3   },
  { 6,  3*PI/2   },
  { 5,  4*PI/3   },
  { 4,  7*PI/6   },
  { 3,  PI       },
  { 2,  5*PI/6   },
  { 1,  2*PI/3   },
  { 12, PI/2     }
};