bool getIdle() {
  int switchState = digitalRead(SWITCH_PIN);
  // return true when idle switch is off (as in the robot should be idle)
  // return false when idle switch is on (as in the robot should run)
  return (switchState == LOW);
};

//IMU gyro;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//IR
IRSensor ir;
int direction, strength;

MotorController mc = MotorController(false);
float tilt;

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

  pinMode(SWITCH_PIN, INPUT);

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
  tilt = event.orientation.x;

  tilt = 360 - tilt;

  //ir
  ir.readData();
  direction = ir.getDirection();
  strength = ir.getStrength();

  Serial.print("\nDirection: "); Serial.print(direction);
  Serial.print("\nStrength: "); Serial.print(strength);

  if (!getIdle()) {
    mc.setSpeed(100);
    mc.runMotors(0, 1, tilt);
  }
  else {
    mc.stopMotors();
  }

  delay(100);
}
