#include <SoftwareSerial.h>

SoftwareSerial bluetooth(0, 1); // RX, TX pins
const int ledPin = 13;

void setup() {
    Serial.begin(9600);
    bluetooth.begin(38400);
    pinMode(ledPin, OUTPUT);
}

void loop() {
    String data;
    int value1;
    float value2;
    String value3;
    
    if (bluetooth.available()){
      data = bluetooth.readStringUntil(';');
      int comma_index = data.indexOf(",");
      int second_comma_index = data.indexOf(",", comma_index + 1);

      if (comma_index != -1 and second_comma_index != -1) {
        value1 = data.substring(0, comma_index).toInt();
        value2 = data.substring(comma_index + 1, second_comma_index).toFloat();
        value3 = data.substring(second_comma_index + 1);
      }
    }
      
    else Serial.println("no bluetooth");
    //command.trim();
    

    if (value3 == "hello") digitalWrite(13, HIGH);
    else digitalWrite(13, LOW);
    
    delay(100);
}
