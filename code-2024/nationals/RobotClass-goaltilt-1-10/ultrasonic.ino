// Ultrasonic Class
class Ultrasonic {
public:
  int trigger_pin;
  int echo_pin;

  Ultrasonic(int TRIGGER_PIN, int ECHO_PIN) {
    trigger_pin = TRIGGER_PIN;
    echo_pin = ECHO_PIN;
  }
  

  int getDistance() {
    long duration, ult_distance;

    // clear trigger pin
    digitalWrite(trigger_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger_pin, LOW);

    // get duration in miliseconds
    duration = pulseIn(echo_pin, HIGH, 20000);
    
    // converet to cm
    ult_distance = (duration / 29) / 2;  // Calculating the ult_distance

    return ult_distance;
  }
};

class PositionSystem {
private: 
  int leftHistory[ULT_HIST_LENGTH] {0};
  int rightHistory[ULT_HIST_LENGTH] {0};

  float avgLeft = 0, avgRight = 0;

  Ultrasonic ultLeft = Ultrasonic(USL_TRIG, USL_ECHO);
  Ultrasonic ultRight = Ultrasonic(USR_TRIG, USR_ECHO);
  Ultrasonic ultBack = Ultrasonic(USB_TRIG, USB_ECHO);

  void checkHistoryEmpty() {
    for (int i = 0; i < ULT_HIST_LENGTH; i++) {
      if (leftHistory[i] != 0) {
        return;
      }
    }
    for (int i = 0; i < ULT_HIST_LENGTH; i++) {
      leftHistory[i] = trueLeft;
      rightHistory[i] = trueRight;
    }
  }

  void updateHistory() {
    for (int i = 0; i < ULT_HIST_LENGTH; i++) {
      if (i != ULT_HIST_LENGTH-1) {
        leftHistory[i] = leftHistory[i+1];
        rightHistory[i] = rightHistory[i+1];
      }
      else {
        leftHistory[i] = trueLeft;
        rightHistory[i] = trueRight;
      }
    }
  }

  float getTrueDist(float tilt, int rawValue) {
    return cos(tilt)*rawValue + ULTRASONIC_TO_ROBOT;
  }

  float getAvgDist(int history[ULT_HIST_LENGTH]) {
    int sum = 0;
    for (int i = 0; i < ULT_HIST_LENGTH; i++) {
      sum += history[i];
    }
    return (sum + 0.0) / ULT_HIST_LENGTH;
  }

  // deciding with ultrasonic to use by checking if a sensor is blocked
  int getLeftDistBlocked() {
    avgLeft = getAvgDist(leftHistory);
    avgRight = getAvgDist(rightHistory);

    leftBlocked = (trueLeft > avgLeft + ULT_RANGE) || (trueLeft < avgLeft - ULT_RANGE);
    rightBlocked = (trueRight > avgRight + ULT_RANGE) || (trueRight < avgRight - ULT_RANGE);
    
    reliable = true;
    if (leftBlocked && rightBlocked) {
      reliable = false;
      return 808;
    }
    else if (leftBlocked) {
      return FIELD_WIDTH - trueRight;
    }
    else {
      return trueLeft;
    }
  }

public:
  float x = 0, y = 0;
  bool sensorBlocked = false;
  bool leftBlocked = false;
  bool rightBlocked = false;

  int rawLeft = 0, rawRight = 0, rawBack = 0;
  float trueLeft = 0, trueRight = 0, trueBack = 0;

  bool reliable = true;

  void update(float tilt) {
    // get raw distances from ultrasonics
    rawLeft = ultLeft.getDistance();
    rawRight = ultRight.getDistance();
    rawBack = ultBack.getDistance();

    trueBack = rawBack*cos(tilt) + ULTRASONIC_TO_ROBOT;

    y = FIELD_LENGTH - trueBack;

    trueLeft = getTrueDist(tilt, rawLeft);
    trueRight = getTrueDist(tilt, rawRight);

    checkHistoryEmpty();

    sensorBlocked = (rawLeft + rawRight + 2*ULTRASONIC_TO_ROBOT) < (FIELD_WIDTH - 30);

    if (!sensorBlocked) {
      x = min(trueLeft, FIELD_WIDTH-trueRight);
    }
    else {
      x = getLeftDistBlocked();
    }

    if (x == 808) {
      x = trueLeft;
    }

    // account for goal
    if (x > 46 && x < 136) {
      y += 17.6;
    }

    updateHistory();
  };
};