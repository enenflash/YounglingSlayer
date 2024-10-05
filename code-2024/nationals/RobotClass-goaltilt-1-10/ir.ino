class IRSensor {
private:
  int strength;
  int direction;
  int dirHistory[DIR_HIST_LENGTH] = {12, 12, 12, 12, 12};
  int strHistory[STR_HIST_LENGTH] = {40, 40, 40, 40, 40};

  // finds the most occuring IR reading from a history of directions
  int mostCommonDirection() {
    int directionCount[12] = {0};
    
    for (int i = 0; i < DIR_HIST_LENGTH; i++) {
      directionCount[dirHistory[i]]++;
    }
    
    int count = directionCount[dirHistory[DIR_HIST_LENGTH-1]];
    int direction = dirHistory[DIR_HIST_LENGTH-1];
    
    for (int i = 0; i < 12; i++) {
      if (directionCount[i] > count) {
        count = directionCount[i];
        direction = i;
      }
    }
    
    return direction;
  };

public:
  void readData() {
    Wire.requestFrom(0x08, 2);
    if (Wire.available()) {
      direction = Wire.read();
      strength = Wire.read();
    }
  };

  int getRawDirection() {
    return direction;
  };
  int getRawStrength() {
    return strength;
  };

  int getFilteredDirection() {
    for (int i = 0; i < DIR_HIST_LENGTH; i++) {
      if (i != DIR_HIST_LENGTH-1) {
        dirHistory[i] = dirHistory[i+1];
      }
      else {
        dirHistory[i] = getRawDirection();
      }
    }
    direction = mostCommonDirection();
    return direction;
  };
};