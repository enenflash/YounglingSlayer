// Authors: Ryan Beikrasouli and Isabelle Tang (Youngling Slayer)
// logic based off of our (Rossmoyne Cosine) standard competition logic (2023)
// note all mesurements are in cm as the ultrasonics mesure in cm
/*
TO DO:
  Use ultrasonics to stay within lines
  Add running forward at start
  Add dribbler code (later)
  Add communication (later)
    Add defence mode
  

*/

// import required packages
#include <iostream>
#include <vector>   // add vectors (equvilent of python lists)
#include <map>      // add std::maps (equivilent of python dictionaries)
#include <iomanip>  // set persision for decimal points
#include <math.h>   // math library
#include <cmath>    // math lirary for trig
#include <any>      // dynamic variable type
#include <Wire.h>   // used for I2C communicaiton with BBR sensor
#include <Adafruit_Sensor.h>  // gyro sensor 
#include <Adafruit_BNO055.h>  // gyro sensor 
#include <utility/imumaths.h> // gyro sensor
#include <LiquidCrystal_I2C.h> // used for LCD
#include <array> // similar to vectors

// Ignoring unimportant warnings
#pragma GCC diagnostic ignored "-Wrange-loop-construct"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wunused-value"

using namespace std;

// motor pins
#define TL_PWM 11
#define TL_DIR 12

#define TR_PWM 1
#define TR_DIR 2

#define BL_PWM 7
#define BL_DIR 8

#define BR_PWM 3
#define BR_DIR 4

// ultrasonic pins
#define LU_TRIG 5
#define LU_ECHO 24
#define RU_TRIG 23
#define RU_ECHO 22
#define BU_TRIG 21
#define BU_ECHO 20

// other pins
#define SWITCH_PIN 30

