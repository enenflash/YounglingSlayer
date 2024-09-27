#include <SoftwareSerial.h>

SoftwareSerial bluetooth(0, 1); // RX, TX pins

void setup() {
    Serial.begin(9600);
    bluetooth.begin(38400);
}
bool looped = false;

void loop() {

    //String command = Serial.readStringUntil('\n');
    int valueToSend = 10;
    float valueToSend2 = 20.2;
    String valueToSend3 = "hello";

    String all_values = String(valueToSend) + "," + String(valueToSend2) + "," + String(valueToSend3) + ";";
    bluetooth.println(all_values); // Customize this command
    if (not looped) {Serial.println(all_values);}
    
    delay(100);
}
