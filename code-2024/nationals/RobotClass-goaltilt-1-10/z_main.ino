Robot bot;
bool idle = true;
bool started = false;

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

void printPos() {
  Serial.print("PosX: ");
  Serial.print(bot.ps.x);
  Serial.print(" PosY: ");
  Serial.println(bot.ps.y);
};

void printOffset() {
  Serial.print("Offset: ");
  Serial.println(bot.offset);
};

void printLine() {
  Serial.print("Line: ");
  Serial.println(bot.lineValue);
};

void printMovement() {
  Serial.print("MoveX: ");
  Serial.print(bot.x);
  Serial.print(" MoveY: ");
  Serial.print(bot.y);
  Serial.print(" Speed: ");
  Serial.println(bot.mc.getSpeed());
};

/* :::::::: MAIN :::::::: */
void loop() {
  // check idle switch
  if (idle) { // change to while loop and remove else?
    if (digitalRead(IDLE_PIN) == HIGH) {
      idle = false;
    }
    else {
      return;
    }
  }

  bot.update();

  // go forward at start
  // if (!started) {
  //   unsigned long startTime = millis();  // Get the start time
  //   while (millis() - startTime < 800) {
  //     mc.runMotors(0, 1, tilt, 0);
  //   }
  //   started = true;
  // }


  Serial.print("Strat: ");

  if (bot.direction == 0) {
    Serial.println("Stop");
    bot.stop();
  }
  else if (bot.direction == 12) {
    Serial.println("Goal target");
    bot.targetGoal();
  }
  // else if (bot.ps.trueBack < 20 && bot.direction != 6) {
  //   Serial.println("Bash");
  //   bot.bashBall();
  // }
  else {
    Serial.println("Get behind ball");
    bot.getBehindBall();
    bot.adjustSpeed();
  }

  // stay within the lines
  bot.stopAtLine();

  printIR();
  printLine();
  printMovement();

  // run robot
  bot.run();
  delay(100);
}

/*
getBehindBall() -> classic getXY
adjustSpeed() -> go slower when ball behind
stopAtLine() -> stay within lines
targetGoal() -> target the goal
manualDefendGoal() -> manually set xy to get to goal
*/