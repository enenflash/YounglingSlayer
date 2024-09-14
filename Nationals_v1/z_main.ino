//IMU gyro;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//IR
IRSensor ir;
int direction, strength, raw_direction, raw_strength;

MotorController mc = MotorController(false);
Ultrasonic left_ultrasonic = Ultrasonic(LU_TRIG, LU_ECHO);
Ultrasonic right_ultrasonic = Ultrasonic(RU_TRIG, RU_ECHO);
Ultrasonic back_ultrasonic = Ultrasonic(BU_TRIG, BU_ECHO);

//Dribbler_Motor DF = Dribbler_Motor(DF_PWM, DF_DIR1, DF_DIR2);
//Dribbler_Motor DB = Dribbler_Motor(DB_PWM, DB_DIR1, DB_DIR2);

//SoftwareSerial bluetooth(0, 1); // TBU // bluetooth with pins 0 and 1

float tilt, triangle_tilt, goal_tilt;
int ult_distance, left_distance, right_distance,back_distance;
int true_left_distance, true_right_distance, true_back_distance;
String ultrasonic_side, position;
array<bool,2> Laser_data;


vector<int> direction_history = {};
vector<int> strength_history = {};
vector<int> left_distance_history;
vector<int> right_distance_history;
vector<int> back_distance_history;

// Setup
void setup() {
  Serial.begin(9600);
  //bluetooth.begin(38400); TBU
  //Serial6.begin(9600); TBU
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
String Colour_Sensor_Data, Bluetooth_Data;
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


  // Ultrasonic
  // get raw distances
    left_distance = left_ultrasonic.get_distance();
    right_distance = right_ultrasonic.get_distance();
    back_distance = back_ultrasonic.get_distance();

    // get true distances (accounting for tilt of robot)
    true_left_distance = Get_True_Distance(left_distance, triangle_tilt) + ULTRASONIC_TO_ROBOT;
    true_right_distance = Get_True_Distance(right_distance, triangle_tilt) + ULTRASONIC_TO_ROBOT;
    true_back_distance = Get_True_Distance(back_distance, triangle_tilt) + ULTRASONIC_TO_ROBOT;
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
      ult_distance = Average(left_distance_history);
      true_left_distance = ult_distance; 
    }

    // turn the right ult_distance into the left ult_distance
    else if (ultrasonic_side == "RIGHT") {
      right_distance_history.insert(right_distance_history.end(), true_right_distance);
      true_right_distance = Average(right_distance_history);
      ult_distance  = FIELD_WIDTH - true_right_distance;
    }

    // ult_distance set to 808 to indicate that neither ultrasonic should be used
    else {
      left_distance_history.insert(left_distance_history.end(), true_left_distance);
      ult_distance = Average(left_distance_history);
      true_left_distance = ult_distance;
    }

    //if (true_left_distance != 0){
    //  left_distance_history.insert(left_distance_history.end(), true_left_distance);
    //}
    
    //ult_distance = Average(left_distance_history);


    position = Find_Position(ult_distance, FIELD_WIDTH, ULTRASONIC_TO_ROBOT);
    
    // Colour sensor TBU
    /*
    Colour_Sensor_Data = Get_Colour_Data(); // get data
    
    // Seperate into variables
    
    int comma_index = data.indexOf(",");
    int second_comma_index = data.indexOf(",", comma_index + 1);
    if (comma_index != -1 and second_comma_index != -1) {
      int value1 = data.substring(0, comma_index).toInt();
      float value2 = data.substring(comma_index + 1, second_comma_index).toFloat();
      String value3 = data.substring(second_comma_index + 1);
    }
    */

    // bluetooth TBU
    /*
    Send_Data(intx,floaty,Stringz);

    Bluetooth_Data = Get_Bluetooth_Data();
    if (Bluetooth_Data != "None") {
      int comma_index = Bluetooth_Data.indexOf(",");
      int second_comma_index = Bluetooth_Data.indexOf(",", comma_index + 1);

      if (comma_index != -1 and second_comma_index != -1) {
        value1 = Bluetooth_Data.substring(0, comma_index).toInt();
        value2 = Bluetooth_Data.substring(comma_index + 1, second_comma_index).toFloat();
        value3 = Bluetooth_Data.substring(second_comma_index + 1);
      }
    }
    else {
      value1 = 0;
      value2 = 0;
      value3 = "None";
    }
    */


  if (true) { // TBU add back idle switch
    if (!run_forward) {
      unsigned long startTime = millis();  // Get the start time
      while (millis() - startTime < 800) {
        mc.runMotors(0, 1, tilt);
      }
      run_forward = true;
    }

    if (in({8,7,6,5,4},direction)) {mc.setSpeed(MAX_SPEED-30);}
    else {mc.setSpeed(MAX_SPEED);}
    x = get_xy(direction, ball_distance, ROBOT_TO_BALL, ROBOT_TO_BALL_MULTIPLY, ir_angles)[0];
    y = get_xy(direction, ball_distance, ROBOT_TO_BALL, ROBOT_TO_BALL_MULTIPLY, ir_angles)[1];
    //goal_tilt = Get_Goal_Tilt(true_back_distance, ult_distance, position, FIELD_HEIGHT, ULTRASONIC_TO_ROBOT, FIELD_WIDTH, direction, strength);
    
    
    if (direction == 12 and ball_distance < MIN_BALL_DISTANCE) {
      goal_tilt = Hard_Goal_Tilt(position);
    }
    else {
      goal_tilt = 0;
    }

    // goal tilt with lasers
    /*
    Laser_data = Laser_Check();
    if (in({11,12,1}, direction) and Laser_data[0]) {
      goal_tilt = Hard_Goal_Tilt(position);
    }
    else {
      goal_tilt = 0;
    }

    // run dribblers
    if ( in({11,12,1}, direction) and (ball_distance < MIN_BALL_DISTANCE or Laser_data[0]) ) {
      DF.run(-100);
    }

    else if (in({5,6,7}, direction) and (ball_distance < MIN_BALL_DISTANCE or Laser_data[1]) ){
      DB.run(-100)
    }

    else {
      DF.stop();
      DB.stop();
    }
    */
    
    mc.runMotors(x, y, tilt - (goal_tilt/2));
  }
  
  else {
    mc.stopMotors();
    DF.stop();
    DB.stop();
  }
  digitalWrite(13, HIGH);
  delay(100);
}
