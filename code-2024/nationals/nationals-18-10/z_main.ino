Robot bot;
bool idle = true;

/* :::::::: SETUP :::::::: */
void setup() {
  Serial.begin(9600);
  Serial.println("\nRobot Initialized");

  Serial6.begin(9600);

  pinMode(IDLE_PIN, INPUT);

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

  /* :::::::: ULTR PINS :::::::: */

  pinMode(USL_TRIG, OUTPUT);
  pinMode(USR_TRIG, OUTPUT);
  pinMode(USB_TRIG, OUTPUT);
  pinMode(USL_ECHO, INPUT);
  pinMode(USR_ECHO, INPUT);
  pinMode(USB_ECHO, INPUT);

  pinMode(IDLE_PIN, INPUT_PULLDOWN);

  bot.mc.stopMotors();
  
  // IMU
  if (!bot.bno.begin()) { Serial.print("\nNo BNO055 detected"); }
  else { Serial.print("\nBNO055 Initialized"); }
  bot.bno.setExtCrystalUse(true);

  // IR
  Wire.begin();
}

/* :::::::: DEBUGGING :::::::: */

void printIR() {
  Serial.print("Direction: ");
  Serial.print(bot.direction);
  Serial.print(" Strength: ");
  Serial.println(bot.strength);
};

void printUltrasonic() {
  Serial.print("True Left: ");
  Serial.print(bot.ps.trueLeft);
  Serial.print(" True Right: ");
  Serial.print(bot.ps.trueRight);
  Serial.print(" ps.x: ");
  Serial.println(bot.ps.x);
};

void printMovement() {
  Serial.print("MoveX: ");
  Serial.print(bot.x);
  Serial.print(" MoveY: ");
  Serial.print(bot.y);
  Serial.print(" Speed: ");
  Serial.println(bot.mc.getSpeed());
};

void printLine() {
  Serial.print("Line: ");
  Serial.println(bot.lineValue);
};

/* :::::::: MAIN :::::::: */
void loop() {
  // check idle switch
  if (idle) {
    if (digitalRead(IDLE_PIN) == HIGH) {
      idle = false;
    }
    else {  
      return;
    }
  }

  bot.update();

  if (bot.direction == 12) {
    bot.targetGoal();
    bot.getOffset();
  }
  else {
    bot.getBehindBall();
  }

  bot.stopAtLine();

  Serial.print("Direction: ");
  Serial.print(bot.direction);
  Serial.print(" Strength: ");
  Serial.println(bot.strength);
  
  printUltrasonic();
  printLine();

  bot.adjustSpeed();
  bot.run();
}