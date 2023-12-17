#include <Arduino.h>

SemaphoreHandle_t xSerialSemaphore = xSemaphoreCreateMutex(); // semaphore for serial monitor.

void SerialGuard (void *pvParameters);

TaskHandle_t xSerialTasks = NULL;
void Task1 (void *pvParameters);
void Task2 (void *pvParameters);

void setup () {
  delay(2000);

  xTaskCreatePinnedToCore (
    SerialGuard,
    "guard1",
    1000,
    NULL,
    10,
    NULL,
    1
  );
}

void loop () {

}

void SerialGuard (void *pvParameters) {
  Serial.begin(9600);

  xTaskCreatePinnedToCore (
    Task1,
    "serial1",
    1000,
    NULL,
    10,
    &xSerialTasks,
    1
  );

  xTaskCreatePinnedToCore (
    Task2,
    "serial2",
    1000,
    NULL,
    11,
    &xSerialTasks,
    1
  );

  vTaskDelay((TickType_t) 10000 / portTICK_PERIOD_MS);

  vTaskDelete(xSerialTasks);
  Serial.end();
  vTaskDelete(NULL);
}

void Task1 (void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 10) == pdTRUE) {
      Serial.println();
      for (int i = 0; i < 5; i++) {
        Serial.println("This is it.");
        vTaskDelay((TickType_t) 500 / portTICK_PERIOD_MS);
      }
      xSemaphoreGive(xSerialSemaphore);
      vTaskDelay((TickType_t) 500 / portTICK_PERIOD_MS);
    }
  }
  
  vTaskDelete(NULL);
}

void Task2 (void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 10) == pdTRUE) {
      Serial.println();
      for (int i = 0; i < 5; i++) {
        Serial.println("Hoc et est.");
        vTaskDelay((TickType_t) 500 / portTICK_PERIOD_MS);
      }
      xSemaphoreGive(xSerialSemaphore);
      vTaskDelay((TickType_t) 500 / portTICK_PERIOD_MS);
    }
  }

  vTaskDelete(NULL);
}