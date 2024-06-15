// Authors: Ryan Beikrasouli and Isabelle Tang (Youngling Slayer)
// logic based off of our (Rossmoyne Cosine) standard competition logic (2023)
// note all mesurements are in cm as the ultrasonics mesure in cm
// NOTE run forward at start, lcd and laser codes have been commented out
#include <iostream>
#include <vector>   // add vectors (equvilent of python lists)
#include <map>      // add std::maps (equivilent of python dictionaries)
#include <iomanip>  // set persision for decimal points
#include <math.h>   // math library
#include <cmath>    // math library for trig
#include <any>      // dynamic variable type
#include <Wire.h>   // used for I2C communicaiton with BBR sensor
#include <Adafruit_Sensor.h>  // gyro sensor 
#include <Adafruit_BNO055.h>  // gyro sensor 
#include <utility/imumaths.h> // gyro sensor
#include <LiquidCrystal_I2C.h> // used for LCD

// Ignoring unimportant warnings
//#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wrange-loop-construct"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

using namespace std;

// Initialising gyro
Adafruit_BNO055 bno = Adafruit_BNO055(55);

// PINS
const int FRONT_LDR_PIN = A0; // (pin 14)
const int SWITCH_PIN = 30;
LiquidCrystal_I2C lcd(0x3F, 16, 2); // LCD

// ############################### FUNCTIONS ###############################

  // find max value in vector
  int16_t find_max(vector<int16_t> vec) {
    int16_t max = vec[0];

    for (unsigned i = 0; i < vec.size(); i++) {
      if (vec[i] > max) {
        max = vec[i];
      }
    }

    return max;
  }

  // find min value in vector
  int16_t find_min(vector<int16_t> vec) {
    int16_t min = vec[0];

    for (unsigned i = 0; i<vec.size(); i++) {
      if (vec[i] < min) {
        min = vec[i];
      }
    }

    return min;
  }

  // find most common value in vector
  int16_t Most_Common(vector<int16_t> vec){
    int16_t Count = 0;
    int16_t item = 0;

    for (unsigned i = 0; i<vec.size(); i++) {
        if (count(vec.begin(), vec.end(), vec[i]) > Count) {
          item = vec[i];
          Count = count(vec.begin(), vec.end(), vec[i]);
        }
    }
    return item;
    
  }

  long Most_Common(vector<long> vec) {
    int16_t Count = 0;
    long item = 0;

    for (unsigned i = 0; i<vec.size(); i++) {
        if (count(vec.begin(), vec.end(), vec[i]) > Count) {
          item = vec[i];
          Count = count(vec.begin(), vec.end(), vec[i]);
        }
    }
    return item;
    
  }

  // check if element is in vector
  bool in(vector<int16_t> var, int16_t element) {
    vector<int16_t>::iterator it = find(var.begin(), var.end(), element);

    if (it != var.end()) {
      return true;
    }

    else {
      return false;
    }
  }

  // convert degree to radians
  double deg_to_rad(double deg) {
    return deg * M_1_PI / 180;
  }

  // convert ratio of speed of different motors to percentage
  vector<int16_t> raw_to_run(vector<int16_t> raw, int16_t speed) {
    int16_t max_raw = find_max(raw);
    int16_t min_raw = find_min(raw);
    int16_t highest;

    // finding the highest percentage
    if (max_raw > min_raw * -1) {
      highest = max_raw;
    }

    else {
      highest = min_raw * -1;
    }

    // setting highest percentage as 100% of speed and scalling everything reletive to it
    if (highest != 0) {
      vector<int16_t> run = {};
      for (unsigned i = 0; i<raw.size(); i++) {
        run.insert(run.end(), round(raw[i] / highest * speed));
      }
      return run;
    }

    else {
      return { 0, 0, 0, 0 };
    }
  }

  // determines position of robot and returns string of position
  string find_position(int16_t true_distance, string ultrasonic_side) {
    const int16_t HALF_WIDTH = 91;
    
    // right ultrasonic
    if (ultrasonic_side == "RIGHT"){
      if (HALF_WIDTH - 20 <= true_distance and true_distance <= HALF_WIDTH + 20) {
        return "CENTER";
      }

      else if (true_distance < HALF_WIDTH) {
        return "RIGHT";
      }

      else {
        return "LEFT";
      }
    }

    // left ultrasonic
    else if (ultrasonic_side == "LEFT"){
      if (HALF_WIDTH - 20 <= true_distance and true_distance <= HALF_WIDTH + 20) {
        return "CENTER";
      }

      else if (true_distance < HALF_WIDTH) {
        return "LEFT";
      }

      else {
        return "RIGHT";
      }
    }

    else {
      return "CENTER";
    }
  }

  // determine the x y coordinants required to travel to
  vector<double> get_xy(int16_t direction, int16_t ball_distance, int16_t ROBOT_TO_BALL, int16_t ROBOT_TO_BALL_MULTIPLY, std::map<int16_t, double_t> sine, std::map<int16_t, double_t> cosine) {
    double x = 0;
    double y = 0;

    // if ball behind robot move to side instead
    if (in({ 2, 10, 11, 1, 3, 9 }, direction)) {
      if (ball_distance < 23 or in({ 3, 9 }, direction)) {
        y = sine[direction] * ball_distance - ROBOT_TO_BALL;
      }

      else {
        y = sine[direction] * ball_distance - (ROBOT_TO_BALL / 1.5);
      }
    }

    else {
      y = sine[direction] * ball_distance;
    }

    if (in({ 4, 5, 6 }, direction)) {
      x = cosine[direction] * ball_distance + (ROBOT_TO_BALL * ROBOT_TO_BALL_MULTIPLY);
    }

    else if (in({ 7, 8 }, direction)) {
      x = cosine[direction] * ball_distance - (ROBOT_TO_BALL * ROBOT_TO_BALL_MULTIPLY);
    }

    else if (in({ 3, 9, 10, 2, 1, 11 }, direction)) {
      x = cosine[direction] * ball_distance;
    }

    else if (direction == 0) {
      x = 0;
      y = 0;
    }

    else if (direction == 12) {
      x = 0;
    }

    return { x, y };
  }

  // determine tilt based on ball direction and position
  int goal_tilt(int16_t direction, int16_t ball_distance, string pos){//, bool got_ball) {
    if (pos == "CENTER") {
      return 0;
    }

    if (pos == "LEFT") {// and got_ball) {
      return  25;
    }

    if (pos == "RIGHT") {// and got_ball) {
      return  -25;
    }

    return 0;
  }

  // determine which ultrasonic (left/right) to use
  string ultrasonic_side_chooser(int16_t left_distance, int16_t right_distance, vector<int16_t> left_distance_history, vector<int16_t> right_distance_history ) {
    const int16_t FIELD_WIDTH = 182;
    const int16_t ULRASONIC_TO_ROBOT = 9;
    bool sensor_blocked;
    
    // determine if one of the sensors is being blocked by an obstical
    if ((left_distance + right_distance + 2 * ULRASONIC_TO_ROBOT) < FIELD_WIDTH - 20) {
      sensor_blocked = true;
    }

    else {
      sensor_blocked = false;
    }

    if (sensor_blocked) {
      
      // most common left and right distances

      int16_t common_left_distance = Most_Common(left_distance_history);
      int16_t common_right_distance = Most_Common(right_distance_history);

      // determine which (if any) sensor to believe

      // right sensor blocked
      if (right_distance > common_right_distance + 10 and not (left_distance > common_left_distance + 10)) {
        return "LEFT";
      }

      // left sensor blocked
      else if (left_distance > common_left_distance + 10 and not (right_distance > common_right_distance + 10)) {
        return "RIGHT";
      }

      // both blocked
      else {
        return "NONE";
      }

    }
    
    // if sensor is not blocked return sensor with shortest distance
    else {
      if (left_distance < right_distance) {
        return "LEFT";
      }

      else {
        return "RIGHT";
      }
    }
  }

  // check if idle switch is on (running) or off (idle)
  bool Switch_Idle(){
    int switchState = digitalRead(SWITCH_PIN);

    // return true when idle switch is off (as in the robot should be idle)
    if (switchState == LOW) {
      return true;
    }
    
    // return false when idle switch is on (as in the robot should run)
    else {
      return false;
    }
  }

// ############################### CLASSES ###############################

  class Motor {
    public:
      int direction_pin;
      int pwm_pin;

      void run_motor(int percentage) {
        // determine whether to run clockwise or anti-clockwise
        if (percentage >= 0) {
          digitalWrite(direction_pin, HIGH);
        }

        else {
          digitalWrite(direction_pin, LOW);
        }

        // ensure values above 100 are converted to 100
        if (abs(percentage)>100) {
          if (percentage<0) {
            percentage = -100;
          }

          else {
            percentage = 0;
          }

        }
        // determine speed and return output value
        int speedValue = (100.0 - abs(percentage)) / 100.0 * 255.0;
        analogWrite(pwm_pin, speedValue);
    }
  };

  class Ultrasonic {
    public:
      int trigger_pin;
      int echo_pin;

      int16_t get_distance() {
        long duration, distance;

        // clear trigger pin
        digitalWrite(trigger_pin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigger_pin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigger_pin, LOW);

        // get duration in miliseconds
        duration = pulseIn(echo_pin, HIGH);

        // converet to cm
        distance = (duration / 29) / 2;  // Calculating the distance

        return distance;
      }
  };

// ############################### SETUP ###############################

  void setup() {
  // ############# MOTORS #############

    // Motor PWM Pins
    pinMode(2, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(8, OUTPUT);

    analogWriteFrequency(2, 20000);
    analogWriteFrequency(4, 20000);
    analogWriteFrequency(6, 20000);
    analogWriteFrequency(8, 20000);

    // Motor CCW/CW Pins
    pinMode(1, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(7, OUTPUT);

  // ############# ULTRASONICS #############
    // trigger pins
    pinMode(9, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(13, OUTPUT);

    // echo pins
    pinMode(10, INPUT);
    pinMode(12, INPUT);
    pinMode(14, INPUT);

  // ############# BBR SETUP #############
    Wire.begin();
  // ############# GYRO SETUP #############
    if (!bno.begin()) {
      // problem connecting
      Serial.print("No BNO055 (Gyro) detected!");
      while(1);
    }

    bno.setExtCrystalUse(true);
  // ############# LCD #############
    lcd.init();
    lcd.backlight();
  // ############# OTHER PINS #############
    pinMode(SWITCH_PIN, INPUT_PULLDOWN); // switch pin
  // printing to computer
    //Serial.begin(9600);
  }

// ############################### MAIN LOOP ###############################
  void loop() {

  // ########################## VARIABLES ##########################

    // Distance between ir and ball center (in strength units)
    const int16_t ROBOT_TO_BALL = 20;
    const int16_t ROBOT_TO_BALL_MULTIPLY = 1;

    // history arrays for increased accuracy
    vector<int16_t> history = {};
    vector<int16_t> distance_history = {};
    vector<int16_t> strength_history = {};
    vector<int16_t> left_distance_history = {};
    vector<int16_t> right_distance_history = {};

    // speed %
    int16_t speed = 60;

    // Distance from ultrasonic to robot edge FIX IN FUTURE
    const int16_t ULRASONIC_TO_ROBOT = 9;

    // String position of robot
    string position = "CENTER";

    // Motor Arrays
    vector<int16_t> motor_ratio = { 0, 0, 0, 0 };
    vector<int16_t> motor_percent = { 0, 0, 0, 0 };

    // hard coded cosine values (x)
    std::map<int16_t, double> cosine = {
      { 0, 0 },
      { 11, 0.5 },
      { 10, 0.86602540378 },
      { 9, 1 },
      { 8, 0.86602540378 },
      { 7, 0.5 },
      { 6, 0 },
      { 5, -0.5 },
      { 4, -0.86602540378 },
      { 3, -1 },
      { 2, -0.86602540378 },
      { 1, -0.5 },
      { 12, 0 }
    };

    // hard coded sine vales (y)
    std::map<int16_t, double> sine = {
      { 0, 0 },
      { 11, 0.86602540378 },
      { 10, 0.5 },
      { 9, 0 },
      { 8, -0.5 },
      { 7, -0.86602540378 },
      { 6, -1 },
      { 5, -0.86602540378 },
      { 4, -0.5 },
      { 3, 0 },
      { 2, 0.5 },
      { 1, 0.86602540378 },
      { 12, 1 }
    };
  
    // seperate robot settings (FUTURE)
      /*
      bool button_pressed;
      while(not button_pressed) {}
      if (button A pressed) {name = "anakin";} 
      else if (button B pressed) {name = "obiwan";}
      */

    // ############# MOTOR OBJECTS #############
      Motor bl_motor;
      bl_motor.direction_pin = 1;
      bl_motor.pwm_pin = 2;

      Motor br_motor;
      br_motor.direction_pin = 3;
      br_motor.pwm_pin = 4;

      Motor tl_motor;
      tl_motor.direction_pin = 5;
      tl_motor.pwm_pin = 6;

      Motor tr_motor;
      tr_motor.direction_pin = 7;
      tr_motor.pwm_pin = 8;

    // ############# ULTRASONIC OBJECTS #############
      Ultrasonic left_ultrasonic;
      left_ultrasonic.trigger_pin = 9;
      left_ultrasonic.echo_pin = 10;

      Ultrasonic right_ultrasonic;
      right_ultrasonic.trigger_pin = 11;
      right_ultrasonic.echo_pin = 12;

      Ultrasonic back_ultrasonic;
      back_ultrasonic.trigger_pin = 13;
      back_ultrasonic.echo_pin = 14;

  // ########################## IDLE ##########################
    bool idle = Switch_Idle();

    // while idle do nothing
    while (idle) {
      tl_motor.run_motor(0);
      tr_motor.run_motor(0);
      bl_motor.run_motor(0);
      br_motor.run_motor(0);
      idle = Switch_Idle();
    }
  
  
  // ########################## MATCH BEGINNING ##########################
    // go straight forward at beginning of match FIX IN FUTURE
    static uint32_t startTime = millis();

    // run for 2 seconds (note 2 seconds is slow)
    /*
    while (millis() - startTime < 2000) {
      // speed up linearly
      if (speed < 100) {
        speed = speed + 10;
      }
    
      // get tilt
      sensors_event_t event;
      bno.getEvent(&event);
      int16_t tilt = round(event.orientation.x);

      // convert to ±180˚
      if (tilt > 180) {
        tilt = -(360-tilt);
      }
      
      // run motors
      tl_motor.run_motor(-100+tilt);  
      tr_motor.run_motor(100+tilt);   
      bl_motor.run_motor(-100+tilt);  
      br_motor.run_motor(100+tilt);   
    }
    */

  // ########################## INFINITE LOOP ##########################

    // ########################## VARIABLES USED IN INFINITE LOOP ##########################
      int16_t direction = 0;
      int16_t direction_raw = 0;
      //int16_t strength_raw = 0;
      int16_t strength = 0;
      string mode = "attack";
      string name;
      //bool back = false;
      //bool too_close = false;
      //bool ball_follow = false;
      int16_t offset = 0;
      //int16_t field_width = 182;
      //int16_t oth_dir = 0;
      //int16_t oth_str = 0;
      //int16_t oth_dis = 0;
      string oth_mode = "attack";
      int16_t true_distance;
      int16_t x = 0;
      int16_t y = 0;
      //int16_t dis_from_center = 0;
      int16_t back_distance = 0;
      int16_t tilt = 0;
      int16_t front_ldr;
      int16_t triangle_tilt = 0;
      int16_t distance = 0;
      int16_t left_distance = 0;
      int16_t right_distance = 0;
      bool got_ball = false;
      string ultrasonic_side = "RIGHT";
      vector<int16_t> ir_seeker = {};

    // ########################## INFINITE LOOP ##########################
      while (not idle) {
      // ########################## INPUTS ##########################
        // ###### CHECK IDLE ######
          idle = Switch_Idle();
  
        // ###### LINEAR ACCELERATION ######
          if (speed < 100) {
          speed = speed + 10;
          }
        // ###### GYRO INPUT ######
          // get tilt
          sensors_event_t event;
          bno.getEvent(&event);
          tilt = round(event.orientation.x);
          if (tilt < 5 and tilt > -5) {
            tilt = 0;
          }

          // convert to ±180˚
          if (tilt > 180) {
            tilt = -(360-tilt);
          }

          // tilt value used for trig calculations
          triangle_tilt = abs(tilt);
    
        // ###### ULTRASONIC INPUT ######
    
          // decide which ultrasonic to use
          left_distance = left_ultrasonic.get_distance();
          right_distance = right_ultrasonic.get_distance();
          back_distance = back_ultrasonic.get_distance();

          left_distance_history.insert(left_distance_history.begin(), left_distance);
          right_distance_history.insert(right_distance_history.begin(), right_distance);

          if (left_distance_history.size() > 5) { left_distance_history.erase(left_distance_history.begin()); }
          if (right_distance_history.size() > 5) { right_distance_history.erase(right_distance_history.begin()); }

          ultrasonic_side = ultrasonic_side_chooser(left_distance, right_distance, left_distance_history, right_distance_history);
          
          // use left ultrasonic
          if (ultrasonic_side == "LEFT") {
            distance = left_distance;
          }

          // use right ultrasonic
          else if (ultrasonic_side == "RIGHT"){
            distance = right_distance;
          }

          // use neither
          else {
            distance = 808;
          }

          true_distance = (distance + ULRASONIC_TO_ROBOT) * cos(deg_to_rad(tilt));
          string position = find_position(true_distance, ultrasonic_side);

        // ###### IR INPUT ######
          Wire.requestFrom(0x10 / 2, 2);
          while (Wire.available()) {
            direction_raw = Wire.read();
            strength = Wire.read();
          }

          // ###### Direction history ######
            history.insert(history.begin(), direction_raw);
            if (history.size() > 3) { history.erase(history.begin()); }

          // ###### Most common direction [direction, count] ######
            direction = Most_Common(history);
            
            // ball distance in strength units
            int16_t ball_distance = 100 - strength;

        // ###### LASER CHECK ######
          /*
          front_ldr = analogRead(FRONT_LDR_PIN);

          // if lazer is triggered and ball is infront
          if (front_ldr < 1030 and in({2,1,12,11,10},direction)){
            got_ball = true;
            direction = 12;
            strength = 80;
            ball_distance = 20;
          }

          else {
            got_ball = false;
          }
          */
          

        // ###### FUTURE STUFF ######
          /*
          // broadcast data FIX IN FUTURE
          int16_t password = 1234;
          vector<int16_t> data = {};  // get data here

          if (data.size() > 0 and data[0] == password) {
            oth_dir = data[1];
            oth_str = data[2];
            oth_dis = data[3];
            oth_mode = data[4];  // might need to set modes by ints to be able to send single vector
          }

          else {
            oth_dir = 0;
            oth_str = 0;
            oth_dis = 0;
            oth_mode = "None";
          }

          // defence situations

          // Situation 1: ball behind other robot and far infront current robot
          if (in({ 3, 5, 6, 7, 8 }, oth_dir) and in({ 12, 1, 11, 2, 3, 9 }, direction) and ball_distance > 35) {
            mode = "defend";
          }

          // Situation 2: if both robots are close and going for ball (anakin has priority)
          else if (oth_dir != 0 and direction - 2 <= oth_dir and oth_dir <= direction + 3 and strength - 10 <= oth_str and oth_str <= strength + 10 and name == "obiwan") {
            mode = "defend";
          }

          // Situation 3: if other robot is going for goal
          else if (in({ 12, 11, 1 }, oth_dir) and oth_str >= 75 and direction != 12 and strength < oth_str + 5) {
            mode = "defend";
          }

          // if none are met go to attack mode
          else {
            mode = "attack";
          }
          */

      // ########################## CALCULATIONS ##########################

        // slow speed when getting close to ball and not infront
        if (strength > 65 and not in({ 2, 1, 12, 11, 10 }, direction)) {
        speed = 60;
        }

        if ((distance == 808 or distance == 807) and (tilt > 1 or tilt < 1)) {
          true_distance = distance_history[0];
        }

        else {
          true_distance = distance * cos(deg_to_rad(triangle_tilt)) + ULRASONIC_TO_ROBOT;
        }

        // add distance to distance history
        if (distance != 808 and distance != 807) { distance_history.insert(distance_history.begin(), true_distance); }

        if (distance_history.size() > 3) { distance_history.erase(distance_history.begin()); }

        // get x y
        x = get_xy(direction, ball_distance, ROBOT_TO_BALL, ROBOT_TO_BALL_MULTIPLY, sine, cosine)[0];
        y = get_xy(direction, ball_distance, ROBOT_TO_BALL, ROBOT_TO_BALL_MULTIPLY, sine, cosine)[1];

        offset = goal_tilt(direction, ball_distance, position);// got_ball);
      
        // GOAL DEFENCE (FUTURE)
          /*
          if (mode == "defend") {
            // go to attack if ball is infront and close
            if (direction == 12 and strength > 73) {
              mode = "attack";
            }

            // go to attack if ball is behind to avoid own goals
            else if (in({ 4, 5, 6, 7, 8 }, direction)) {
              mode = "attack";
            }
        
            else {
              dis_from_center = -(true_distance - 910);

              if (not ball_follow) {
                if (dis_from_center > 200 or dis_from_center < 200) {
                  x = -dis_from_center;
                  y = 0;
                }

                else {
                  speed = 60;
                  y = -back_distance + 400;
                }

                if (-200 <= dis_from_center and dis_from_center <= 200 and 200 <= back_distance and back_distance <= 500) {
                  ball_follow = true;
                }
              }

              else {
                speed = 80;
                y = 0;
                
                if (dis_from_center <= -200) {
                  x = 1;
                } 
                
                else if (dis_from_center >= 200) {
                  x = -1;
                }
              }
            }
          }

          else {
            mode == "attack";
          }
          */

      
      // ########################## OUTPUT ##########################
        lcd.setCursor(0, 0);
        lcd.print("Direction: ");
        lcd.print(direction);
        lcd.setCursor(0, 1);
        lcd.print("Strength: ");
        lcd.print(strength);

        motor_ratio = { (int16_t)(-x - y), (int16_t)(-x + y), (int16_t)(x - y), (int16_t)(x + y) };
        motor_percent = raw_to_run(motor_ratio, speed);
        
        lcd.setCursor(0, 0);
        lcd.print("THIS IS IN MAIN LOOP");

        // run motors
        tl_motor.run_motor(10);//motor_percent[0] + (tilt / 2) - (offset / 2));
        tr_motor.run_motor(10);//motor_percent[1] + (tilt / 2) - (offset / 2));
        bl_motor.run_motor(10);//motor_percent[2] + (tilt / 2) - (offset / 2));
        br_motor.run_motor(10);//motor_percent[3] + (tilt / 2) - (offset / 2));

        // light up pin on teensy
        //digitalWrite(13, HIGH);
        
        // delay 100 miliseconds before next loop
        delay(100);
        
      }
  }
#pragma GCC diagnostic pop