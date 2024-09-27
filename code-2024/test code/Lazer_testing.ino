const int ldrPin = A0; // Connect the LDR to analog pin A0

void setup() {
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  int ldrValue = analogRead(ldrPin); // Read LDR value (0-1023)
  
  // Print the LDR value to the serial monitor
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);
  digitalWrite(13, HIGH);
  delay(1000);

}