#include <Arduino.h>

// The board schematic connects VCC -> resistor -> LED -> PB2, so LOW turns
// the LED on.
void setup() {
  pinMode(PB2, OUTPUT);
  digitalWrite(PB2, LOW);
}

void loop() {
}
