#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
  
Adafruit_BNO055 bno = Adafruit_BNO055(55);

void setup() {
  Serial.begin(9600);
  Serial.println("Orientation Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  bno.setExtCrystalUse(true);
}

void loop() {
  while (true) {
  /* Get a new sensor event */ 
  sensors_event_t event;
  bno.getEvent(&event);
  int16_t tilt = round(event.orientation.x);
  int16_t y = round(event.orientation.y);
  int16_t z = round(event.orientation.z);
  //float tilt = event.orientation.x;
  if (tilt > 180) {
    tilt = -(360-tilt);
  }

  /* Display the floating point data */
  Serial.print("X: ");
  Serial.print(tilt);
  Serial.println("");
  Serial.print("Y: ");
  Serial.print(y);
  Serial.println("");
  Serial.print("Z: ");
  Serial.print(z);
  Serial.println("\n");

  digitalWrite(13, HIGH);
  delay(100);
  
  }
  
}
