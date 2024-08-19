//IMU gyro;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//IR
IRSensor ir;
int direction, strength, raw_direction, raw_strength;

MotorController mc = MotorController(false);
float tilt;

vector<int> direction_history = {};
vector<int> strength_history = {};
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
  strength_history.insert(strength_history.end(), raw_strength);
  if (strength_history.size() > 3) strength_history.erase(strength_history.begin());
  strength = Average(strength_history);

  int ball_distance = 100-strength;

  //Serial.print("\nDirection: "); Serial.print(direction);
  //Serial.print("\nStrength: "); Serial.print(strength);

  if (!getIdle()) {
    mc.setSpeed(100);
    x = get_xy(direction, ball_distance, ROBOT_TO_BALL, ROBOT_TO_BALL_MULTIPLY, ir_angles)[0];
    y = get_xy(direction, ball_distance, ROBOT_TO_BALL, ROBOT_TO_BALL_MULTIPLY, ir_angles)[1];
    mc.runMotors(x, y, tilt);
  }
  else {
    mc.stopMotors();
  }

  delay(100);
}
