#include <Arduino.h>

const int ledPin = BUILTIN_LED; // aka pin 2.
const int ledInterval = 1250;
unsigned long ledTimer;
unsigned long *ledTimerPtr = &ledTimer;
int ledState = LOW;
int* ledStatePtr = &ledState;

TaskHandle_t SampleTask;

int blinkWithoutDelay( int pin, int *statePtr, int interval, unsigned long *timerPtr);
void Task1 (void *pvParameters);

void setup () {
  delay(1000);
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  Serial.print("Main thread is on core ");
  Serial.println(xPortGetCoreID());
  
  ledTimer = millis();

  xTaskCreatePinnedToCore (
    Task1,
    "blink",
    1000,
    NULL,
    10,
    NULL,
    0
  );
}

void loop () {
  
}

void Task1 (void *pvParameters) {
  for (;;) {
    ledState = (ledState == LOW) ? HIGH: LOW;
    digitalWrite(ledPin, ledState);
    vTaskDelay(TickType_t (12500/portTICK_PERIOD_MS));
    Serial.printf("Blink thread is running on core %d\n", xPortGetCoreID());
  }
}