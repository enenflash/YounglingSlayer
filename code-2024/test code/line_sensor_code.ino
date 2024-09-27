#include <iostream>
#include <map>
#include <array>
using namespace std;

#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define ADC0 26

const int minReading = 50;
const int maxReading = 100;

// sensor number, {S3, S2, S1, S0}
std::map<int, array<int, 4>> mpMap = {
  { 1, {0, 1, 0, 0} }, 
  { 2, {0, 1, 1, 0} }, 
  { 3, {0, 1, 0, 1} }, 
  { 4, {1, 0, 1, 1} }, 
  { 5, {1, 0, 1, 0} }, 
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

array<int, 4> mpReference;
array<bool, 16> sensorReadings;

int get_sensor_reading(int sensor_number) {
  mpReference = mpMap[sensor_number];
  digitalWrite(S3, mpReference[0]);
  digitalWrite(S2, mpReference[1]);
  digitalWrite(S1, mpReference[2]);
  digitalWrite(S0, mpReference[3]);
  return analogRead(ADC0);
}

bool isWhite(int sensorReading) {
  return (minReading < sensorReading && sensorReading < maxReading);
}

void print_sensors_raw() {
  for (int i = 0; i < 16; i++) {
    int reading = get_sensor_reading(i);
    Serial.print(i);
    Serial.print(": ");
    Serial.print(reading);
    Serial.println("");
  }
}

void get_all_sensors() {
  for (int i = 0; i < 16; i++) {
    Serial.println(get_sensor_reading(i));
    sensorReadings[i] = isWhite(get_sensor_reading(i));
  }
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
  get_all_sensors();
  
  for (int i = 0; i < 16; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(sensorReadings[i]);
    Serial.println("");
  }

  delay(100);
}