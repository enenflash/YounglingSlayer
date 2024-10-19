#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

using namespace std;

#define USR_TRIG 34
#define USR_ECHO 33
#define USL_TRIG 22
#define USL_ECHO 23
#define USB_TRIG 28
#define USB_ECHO 27

#define FIELD_WIDTH 182
#define FIELD_HEIGHT 243
const int UTR = 10;

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
  Ultrasonic ultLeft = Ultrasonic(USL_TRIG, USL_ECHO);
  Ultrasonic ultRight = Ultrasonic(USR_TRIG, USR_ECHO);
  Ultrasonic ultBack = Ultrasonic(USB_TRIG, USB_ECHO);

  // LEFT, RIGHT, BACK
  int rawDistances[3] = {0};
  float trueDistances[3] = {0};
  float previousDistances[3] = {0};

  float deltaDistances[3] = {0};

  bool reliable = true;
  int usingIndex = 0; 

  float x, y;

  void getRawDistances() {
    rawDistances[0] = ultLeft.getDistance() + UTR;
    rawDistances[1] = ultRight.getDistance() + UTR;
    rawDistances[2] = ultBack.getDistance() + UTR;
  };

  void getTrueDistances(float tilt) { // tilt in radians
    for (int i = 0; i < 3; i++) {
      trueDistances[i] = cos(tilt)*rawDistances[i];
    }
  };

  bool totalWidthCompare() {
    return abs((trueDistances[0]+trueDistances[1])-FIELD_WIDTH) < 20;
  };

  void getDeltaDistances() {
    for (int i = 0; i < 3; i++) {
      deltaDistances[i] = abs(trueDistances[i]-previousDistances[i]);
    }
  };

  void logPreviousDistances() {
    for (int i = 0; i < 3; i++) {
      previousDistances[i] = trueDistances[i];
    }
  };

public:

  void chooseULT() {
    getDeltaDistances();

    if (deltaDistances[0] > 30 && deltaDistances[1] > 30) {
      usingIndex = 0;
      reliable = false;
    }
    else if (deltaDistances[0] > 30) {
      usingIndex = 1;
      reliable = true;
    }
    else if (deltaDistances[1] > 30) {
      usingIndex = 0;
      reliable = true;
    }
    else {
      usingIndex = 0;
      reliable = true;
    }
  };

  void update(float tilt) {
    getRawDistances();
    getTrueDistances(tilt);

    if (!totalWidthCompare()) {
      Serial.println("Choose ULT");
      chooseULT();
    }
    
    if (usingIndex == 0) {
      x = trueDistances[0];
    }
    else if (usingIndex == 1) {
      x = FIELD_WIDTH - trueDistances[1];
    }

    y = FIELD_HEIGHT - trueDistances[2];

    if (totalWidthCompare()) {
      logPreviousDistances();
    }
  };

  void serialPrint() {
    Serial.print("TrueLeft: ");
    Serial.print(trueDistances[0]);
    Serial.print(" TrueRight: ");
    Serial.println(trueDistances[1]);

    Serial.print("Reliable: ");
    Serial.print(reliable);
    Serial.print(" usingIndex: ");
    Serial.println(usingIndex);
  }
};