#include <Arduino.h>

void setup() {
    // Initialize Serial1 with baud rate 9600
    Serial1.begin(9600);

    // Set the TX (pin 4) and RX (pin 5) for Serial1
    // Note: On Raspberry Pi Pico, TX is pin 4 and RX is pin 5 by default
}

void loop() {
    int valueToSend = 13;

    // Send the value over UART
    Serial1.write(valueToSend);

    // Add a delay (optional)
    delay(1000);
}
