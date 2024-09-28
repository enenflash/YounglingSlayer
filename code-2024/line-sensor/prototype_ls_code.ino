// uses 
#include <iostream>
#include <map>
#include <array>
#include <algorithm>
using namespace std;

#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define ADC0 26

// min and max reading can be changed to suit the type of white line
const int minReading = 50;
const int maxReading = 100;

// multiplexer map to access different TCRT-5000
std::map<int, array<int, 4>> mpMap = {
  { 1,  {0, 1, 0, 0} }, 
  { 2,  {0, 1, 1, 0} }, 
  { 3,  {0, 1, 0, 1} }, 
  { 4,  {1, 0, 1, 1} }, 
  { 5,  {1, 0, 1, 0} }, 
  { 6,  {0, 0, 1, 1} }, 
  { 7,  {1, 0, 0, 0} }, 
  { 8,  {1, 0, 0, 1} }, 
  { 9,  {0, 0, 0, 0} }, 
  { 10, {0, 0, 1, 0} }, 
  { 11, {0, 0, 0, 1} }, 
  { 12, {1, 1, 1, 1} }, 
  { 13, {0, 1, 1, 1} }, 
  { 14, {1, 1, 0, 0} }, 
  { 15, {1, 1, 1, 0} },
  { 16, {1, 1, 0, 1} }
};

// sensor coordinates from center of robot (in mm)
std::map<int, array<int, 2>> sensorCoords = {
  { 1,  {0,   40}  },
  { 2,  {35,  61}  },
  { 3,  {0,   70}  },
  { 4,  {-35, 61}  },
  { 5,  {-40, 0}   },
  { 6,  {-61, 35}  },
  { 7,  {-70, 0}   },
  { 8,  {-61, -35} },
  { 9,  {0,   -40} },
  { 10, {-35, -61} },
  { 11, {0,   -70} },
  { 12, {35,  -61} },
  { 13, {40,  0}   },
  { 14, {61,  35}  },
  { 15, {70,  0}   },
  { 16, {61,  -35} }
};

array<int, 4> mpReference;
array<bool, 16> sensorReadings;

// top left right bottom
int probabilityChart[4];
int probabilityLine;
int minPos;

bool allZero;

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
    sensorReadings[i] = isWhite(get_sensor_reading(i));
  }
}

void incrementProbability(int sensorX, int sensorY) {
  if (sensorX < 0) {
    probabilityChart[1]++;
  }
  else if (sensorX > 0) {
    probabilityChart[2]++;
  }
  else {
    probabilityChart[1]++;
    probabilityChart[2]++;
  }

  if (sensorY < 0) {
    probabilityChart[3]++;
  }
  else if (sensorY > 0) {
    probabilityChart[0]++;
  }
  else {
    probabilityChart[3]++;
    probabilityChart[0]++;
  }
}

void decrementProbability(int sensorX, int sensorY) {
  if (sensorX < 0) {
    probabilityChart[1]--;
  }
  else if (sensorX > 0) {
    probabilityChart[2]--;
  }
  else {
    probabilityChart[1]--;
    probabilityChart[2]--;
  }

  if (sensorY < 0) {
    probabilityChart[3]--;
  }
  else if (sensorY > 0) {
    probabilityChart[0]--;
  }
  else {
    probabilityChart[3]--;
    probabilityChart[0]--;
  }
}

void setup() {
  Serial.begin(9600);
  // use Serial1 to send data to Teensy 4.1
  Serial1.begin(9600);

  pinMode(ADC0, INPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
}

void loop() {
  // update sensorReadings
  get_all_sensors();

  // clear probability chart
  for (int i = 0; i < 4; i++) {
    probabilityChart[i] = 0;
  }

  // print sensorReadings
  // for (int i = 0; i < 16; i++) {
  //   Serial.print(i);
  //   Serial.print(": ");
  //   Serial.print(sensorReadings[i]);
  //   Serial.println("");
  // }

  // increment/decrement probability of line position
  allZero = true;
  for (int i = 0; i < 16; i++) {
    if (sensorReadings[i]) {
      incrementProbability(sensorCoords[i][0], sensorCoords[i][1]);
      allZero = false;
    }
    else {
      decrementProbability(sensorCoords[i][0], sensorCoords[i][1]);
    }
  }

  // probability Line:
  // 0: no line
  // 1: top
  // 2: left
  // 3: right
  // 4: bottom
  
  if (allZero) {
    Serial.println("No sensor");
    Serial1.write(0);
  }
  else {
    // picks highest probability
    probabilityLine = 0;
    for (int i = 1; i < 4; i++) {
      if (probabilityChart[i] > probabilityChart[probabilityLine]) probabilityLine = i;
    }

    Serial.println(probabilityLine);
    Serial1.write(probabilityLine+1);
  }

  // could also include finding the distance of the line from the center of the robot here
  
  delay(100);
}
