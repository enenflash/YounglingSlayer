void setup() {
  Serial5.begin(9600); // Initialize Serial1 (TX1)
}

void loop() {
  // Receive data from Raspberry Pi Pico
  if (Serial5.available()) {
      int receivedfloat = Serial5.read();
      Serial.print("Received: ");
      Serial.println(receivedfloat); // Print received character
  }
  //Serial.println(Serial5.available());
}
