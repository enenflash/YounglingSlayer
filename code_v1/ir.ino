class IRSensor {
private:
  int strength;
  int direction;

public:
  void readData() {
    Wire.requestFrom(0x10 / 2, 2);
    while (Wire.available()) {
      direction = Wire.read();
      strength = Wire.read();
    }
  };
  int getDirection() {
    return direction;
  };
  int getStrength() {
    return strength;
  };
};
