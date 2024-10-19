PositionSystem ps;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

float tilt;

void setup() {
  Serial.begin(9600);
  Serial.println("\nRobot Initialized");

  pinMode(USL_TRIG, OUTPUT);
  pinMode(USR_TRIG, OUTPUT);
  pinMode(USB_TRIG, OUTPUT);
  pinMode(USL_ECHO, INPUT);
  pinMode(USR_ECHO, INPUT);
  pinMode(USB_ECHO, INPUT);

  if (!bno.begin()) { Serial.print("\nNo BNO055 detected"); }
  else { Serial.print("\nBNO055 Initialized"); }
  bno.setExtCrystalUse(true);
}

void loop() {
  sensors_event_t event;
  bno.getEvent(&event);
  tilt = event.orientation.x;

  // convert to unit circle tilt
  tilt = (360 - tilt)*PI/180;

  ps.update(tilt); // give ps.update tilt in radians
  ps.serialPrint(); // use this for debugging
}