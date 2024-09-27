#include <Wire.h>
#include <iostream>

using namespace std;
                    
void setup() 
{  
  Wire.begin();
  Serial.begin(9600);
}
                
void loop() 
{    
    Wire.requestFrom(0x10 / 2, 2);
    while (Wire.available())    
    {     
        int c = Wire.read(); // direction is the first byte    
        Serial.print("Best Sensor is: ");    
        Serial.println(c);   
        //smaller number the closer the ball
        int strength = Wire.read(); 
        Serial.print("Strength is: ");    
        Serial.println(strength);          
    }  
    delay(250);
}