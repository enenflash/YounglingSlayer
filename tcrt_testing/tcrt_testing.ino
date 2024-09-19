#include <iostream>
#include <map>
using namespace std;

#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define ADC0 26

#define MINIMUM_READING 80

// sensor number, {S3, S2, S1, S0}
std::map<int, array<int, 4>> Colour_Outputs = {
  { 1, {0, 1, 0, 0}}, 
  { 2, {0, 1, 1, 0}}, 
  { 3, {0, 1, 0, 1} }, 
  { 4, {1, 0, 1, 1} }, 
  { 5, {1, 0, 1, 0}}, 
  { 6, {0, 0, 1, 1} }, 
  { 7, {1, 0, 0, 0} }, 
  { 8, {1, 0, 0, 1} }, 
  { 9, {0, 0, 0, 0} }, 
  { 10, {0, 0, 1, 0} }, 
  { 11, {0, 0, 0, 1} }, 
  { 12, {1, 1, 1, 1} }, 
  { 13, {0, 1, 1, 1} }, 
  { 14, {1, 1, 0, 0} }, 
  { 15, {1, 1, 1, 0} }, 
  { 16, {1, 1, 0, 1} }
};

int get_sensor_reading(int sensor_number) {
  array<int, 4> outputs = Colour_Outputs[sensor_number];
  digitalWrite(S3, outputs[0]);
  digitalWrite(S2, outputs[1]);
  digitalWrite(S1, outputs[2]);
  digitalWrite(S0, outputs[3]);
  int reading = analogRead(ADC0);
  return reading;

}


void setup() {
  Serial.begin(9600);
  pinMode(ADC0, INPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
}

void loop() {
  int reading;
  for (int i = 1; i<17; i++) {
    reading = get_sensor_reading(i);
    if (reading < MINIMUM_READING) {
      Serial.print(i);
      Serial.print(": ");
      Serial.println(reading);
    }
    
    
    
  }

  Serial.println("");

  delay(100);
}
