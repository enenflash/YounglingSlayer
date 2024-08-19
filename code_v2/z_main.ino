//IMU gyro;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//IR
IRSensor ir;

// Motor Controller
MotorController mc = MotorController(false);

Ultrasonic left_ultrasonic = Ultrasonic(LU_TRIG, LU_ECHO);
Ultrasonic right_ultrasonic = Ultrasonic(RU_TRIG, RU_ECHO);
Ultrasonic back_ultrasonic = Ultrasonic(BU_TRIG, BU_ECHO);

// Other Variables
float tilt, norm_tilt, triangle_tilt, tilt_180, goal_tilt;
float x,y;
int direction, strength, raw_direction, raw_strength;
int distance, left_distance, right_distance,back_distance;
int true_left_distance, true_right_distance, true_back_distance;
int speed = 100;
String ultrasonic_side, position;

vector<int> direction_history = {};
vector<int> strength_history = {};
vector<int> left_distance_history;
vector<int> right_distance_history;
vector<int> back_distance_history;

// Setup
void setup() {
  Serial.begin(9600);

  /* :::::::: MOTOR PINS :::::::: */

  pinMode(TL_PWM, OUTPUT);
  pinMode(TR_PWM, OUTPUT);
  pinMode(BL_PWM, OUTPUT);
  pinMode(BR_PWM, OUTPUT);

  analogWriteFrequency(TL_PWM, 20000);
  analogWriteFrequency(TR_PWM, 20000);
  analogWriteFrequency(BL_PWM, 20000);
  analogWriteFrequency(BR_PWM, 20000);

  pinMode(TL_DIR, OUTPUT);
  pinMode(TR_DIR, OUTPUT);
  pinMode(BL_DIR, OUTPUT);
  pinMode(BR_DIR, OUTPUT);

  // ultrasonic pins
  pinMode(LU_TRIG, OUTPUT);
  pinMode(RU_TRIG, OUTPUT);
  pinMode(BU_TRIG, OUTPUT);
  pinMode(LU_ECHO, INPUT);
  pinMode(RU_ECHO, INPUT);
  pinMode(BU_ECHO, INPUT);

  pinMode(SWITCH_PIN, INPUT_PULLDOWN);

  mc.stopMotors();
  
  //imu
  if (!bno.begin()) { Serial.print("\nNo BNO055 detected"); }
  else { Serial.print("\nBNO055 Initialized"); }
  bno.setExtCrystalUse(true);

  //ir
  Wire.begin();
}

void loop() {
  // imu
    sensors_event_t event;
    bno.getEvent(&event);
    norm_tilt = event.orientation.x;

    // give buffer for being straight to avoid unnecessary tilting
    if (norm_tilt > 350 or norm_tilt < 10) {
      norm_tilt = 0;
    }

    tilt = 360 - norm_tilt;
    
    
    // convert tilt to ±180˚
    if (norm_tilt > 180) {
      tilt_180 = -(360-norm_tilt);}
    
    else {tilt_180 = norm_tilt;}
    
    // tilt used for trig calculations
    triangle_tilt = abs(tilt_180);
    

  //ir
    ir.readData();
    raw_direction = ir.getDirection(); 
    direction_history.insert(direction_history.end(), raw_direction);
    if (direction_history.size() > 5) direction_history.erase(direction_history.begin());
    direction = Most_Common(direction_history);
    
    raw_strength = ir.getStrength();
    strength_history.insert(strength_history.end(), raw_strength);
    if (strength_history.size() > 3) strength_history.erase(strength_history.begin());
    strength = Average(strength_history);

    int ball_distance = 100-strength;

  //ultrasonic 
    // get raw distances
    int distance = 808;
    left_distance = left_ultrasonic.get_distance();
    right_distance = right_ultrasonic.get_distance();
    back_distance = back_ultrasonic.get_distance();

    // get true distances (accounting for tilt of robot)
    true_left_distance = Get_True_Distance(left_distance, triangle_tilt);
    true_right_distance = Get_True_Distance(right_distance, triangle_tilt);
    true_back_distance = Get_True_Distance(back_distance, triangle_tilt);
    back_distance_history.insert(back_distance_history.end(), true_back_distance);

    // shorten histories
    if (left_distance_history.size() > 5) { left_distance_history.erase(left_distance_history.begin()); }
    if (right_distance_history.size() > 5) { right_distance_history.erase(right_distance_history.begin()); }
    if (back_distance_history.size() > 5) { back_distance_history.erase(back_distance_history.begin()); }

    true_back_distance = Most_Common(back_distance_history);

    // ultrasonic side
    ultrasonic_side = ultrasonic_side_chooser(true_left_distance, true_right_distance, left_distance_history, right_distance_history, FIELD_WIDTH, ULTRASONIC_TO_ROBOT);

    if (ultrasonic_side == "LEFT") {
      right_distance_history.insert(right_distance_history.end(), true_left_distance);
      distance = Most_Common(right_distance_history);
    }

    // turn the right distance into the left distance
    else if (ultrasonic_side == "RIGHT") {
      right_distance_history.insert(right_distance_history.end(), true_right_distance);
      distance = FIELD_WIDTH - 2 * ULTRASONIC_TO_ROBOT - Most_Common(right_distance_history);
    }

    // distance set to 808 to indicate that neither ultrasonic should be used
    else {
      distance = 808;
    }

    position = Find_Position(distance, FIELD_WIDTH);

  // main loop
    if (!getIdle()) {
      // calculations
      

      // speed calculations
      if (speed < 100) {speed = speed + 10;}

      if (in({4,5,6,7,8}, direction) and strength < 25) {
        speed = 70;
      }
      
      else if (direction == 12) {
        speed = 100; 
      }
      
      // get the x, y coordinants to head to
      mc.setSpeed(100);//speed);
      x = get_xy(direction, ball_distance, ROBOT_TO_BALL, ROBOT_TO_BALL_MULTIPLY, ir_angles)[0];
      y = get_xy(direction, ball_distance, ROBOT_TO_BALL, ROBOT_TO_BALL_MULTIPLY, ir_angles)[1];
      
      
      // calculate value required to tilt to goal
      if (ball_distance < 25 and in({1,12,11}, direction)) {
        goal_tilt = Get_Goal_Tilt(true_back_distance, distance, position, FIELD_HEIGHT, ULTRASONIC_TO_ROBOT, FIELD_WIDTH);
      }

      else { goal_tilt = 0;}

      goal_tilt = 0;
      //if (tilt_180 + 15 > -goal_tilt and -goal_tilt > tilt_180 - 15) {goal_tilt = 0;}
      
      // output
      
      mc.runMotors(0,0,tilt, goal_tilt/2);//x, y, tilt);// - goal_tilt); 
    }

    else {
      mc.stopMotors();
    }

    delay(100);
}
