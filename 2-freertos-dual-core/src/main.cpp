#include <Arduino.h>

const int ledPin = BUILTIN_LED; // aka pin 2.
const int ledInterval = 250;
unsigned long ledTimer;
unsigned long *ledTimerPtr = &ledTimer;
int ledState = LOW;
int* ledStatePtr = &ledState;

void blinkWithoutDelay(int pin, int *statePtr, int interval, unsigned long *timerPtr);

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  
  ledTimer = millis();
}

void loop() {
  blinkWithoutDelay(ledPin, ledStatePtr, ledInterval, ledTimerPtr);
}

void blinkWithoutDelay (int pin, int *statePtr, int interval, unsigned long *timerPtr) {
  if (millis() - *timerPtr >= interval) {
    *statePtr = (*statePtr == LOW) ? HIGH : LOW;
    digitalWrite(pin, *statePtr);
    *timerPtr = millis();

    Serial.printf("%d\n", *statePtr);
  }
}