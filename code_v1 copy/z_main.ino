//IMU gyro;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//IR
IRSensor ir;
int direction, strength, raw_direction, raw_strength;

MotorController mc = MotorController(false);
Ultrasonic left_ultrasonic = Ultrasonic(LU_TRIG, LU_ECHO);
Ultrasonic right_ultrasonic = Ultrasonic(RU_TRIG, RU_ECHO);
Ultrasonic back_ultrasonic = Ultrasonic(BU_TRIG, BU_ECHO);

float tilt, triangle_tilt;
int distance, left_distance, right_distance,back_distance;
int true_left_distance, true_right_distance, true_back_distance;
String ultrasonic_side, position;


vector<int> direction_history = {};
vector<int> strength_history = {};
vector<int> left_distance_history;
vector<int> right_distance_history;
vector<int> back_distance_history;

// Setup
void setup() {
  Serial.begin(9600);
  Serial.println("\nRobot Initialized");
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

  pinMode(BU_TRIG, OUTPUT);
  pinMode(LU_TRIG, OUTPUT);
  pinMode(RU_TRIG, OUTPUT);
  pinMode(BU_ECHO, INPUT);
  pinMode(LU_ECHO, INPUT);
  pinMode(RU_ECHO, INPUT);

  pinMode(SWITCH_PIN, INPUT_PULLDOWN);

  mc.stopMotors();
  
  //imu
  if (!bno.begin()) { Serial.print("\nNo BNO055 detected"); }
  else { Serial.print("\nBNO055 Initialized"); }
  bno.setExtCrystalUse(true);

  //ir
  Wire.begin();
}

float x,y;
bool run_forward = false;
void loop() {
  // imu
  sensors_event_t event;
  bno.getEvent(&event);
  tilt = event.orientation.x;

  tilt = 360 - tilt;

  //ir
  ir.readData();
  raw_direction = ir.getDirection(); 
  direction_history.insert(direction_history.end(), raw_direction);
  if (direction_history.size() > 5) direction_history.erase(direction_history.begin());
  direction = Most_Common(direction_history);
  
  raw_strength = ir.getStrength();
  if (raw_strength != 0) {strength_history.insert(strength_history.end(), raw_strength);}
  
  if (strength_history.size() > 3) strength_history.erase(strength_history.begin());
  strength = Average(strength_history);

  int ball_distance = 100-strength;

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

    true_back_distance = Average(back_distance_history);

    // ultrasonic side
    ultrasonic_side = ultrasonic_side_chooser(true_left_distance, true_right_distance, left_distance_history, right_distance_history, FIELD_WIDTH, ULTRASONIC_TO_ROBOT);

    if (ultrasonic_side == "LEFT") {
      left_distance_history.insert(left_distance_history.end(), true_left_distance);
      distance = Average(left_distance_history);
      true_left_distance = distance;
    }

    // turn the right distance into the left distance
    else if (ultrasonic_side == "RIGHT") {
      right_distance_history.insert(right_distance_history.end(), true_right_distance);
      true_right_distance = Average(right_distance_history);
      distance  = FIELD_WIDTH - 2 * ULTRASONIC_TO_ROBOT - true_right_distance;
    }

    // distance set to 808 to indicate that neither ultrasonic should be used
    else {
      distance = 808;
    }

    position = Find_Position(distance, FIELD_WIDTH, ULTRASONIC_TO_ROBOT);

  if (true) {
    //if (!run_forward) {
      //unsigned long startTime = millis();  // Get the start time
      //while (millis() - startTime < 800) {
      //  mc.runMotors(0, 1, tilt);
      //}
      //run_forward = true;
    //}

    if (in({8,7,6,5,4},direction)) {mc.setSpeed(70);}
    else {mc.setSpeed(100);}
    x = get_xy(direction, ball_distance, ROBOT_TO_BALL, ROBOT_TO_BALL_MULTIPLY, ir_angles)[0];
    y = get_xy(direction, ball_distance, ROBOT_TO_BALL, ROBOT_TO_BALL_MULTIPLY, ir_angles)[1];
    //x = Line_Check(distance, x);
    mc.runMotors(x, y, tilt);
  }
  
  else {
    mc.stopMotors();
  }

  delay(100);
}
