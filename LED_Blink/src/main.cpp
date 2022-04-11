#include <Arduino.h>

#define LED 2

void setup() {
  Serial.begin(9600); //sets baud rate to this
  pinMode(LED, OUTPUT); //sets pin "LED" to output
}

void loop() {
  digitalWrite(LED, HIGH); //sets pin "LED" high
  Serial.println("LED is on");  //prints with new line
  delay(1000);  //delay is in ms
  digitalWrite(LED, LOW); //sets pin low
  Serial.println("LED is off");
  delay(1000);
}