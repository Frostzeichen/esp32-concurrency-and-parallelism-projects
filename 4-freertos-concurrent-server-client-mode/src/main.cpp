#include <Arduino.h>
#include <WiFi.h>
#include "prepareResponse.h"

String SSID = "Nokia 3.4";
String PASS = "Qwerty123";
int PORT = 80;

SemaphoreHandle_t xSerialSemaphore = NULL;
SemaphoreHandle_t xWifiSemaphore = NULL;

TaskHandle_t startupRoutine = NULL;
TaskHandle_t networkRoutine = NULL;

void Blink3 (void *pvParameters);
void SerialSetup (void *pvParameters);
void StartupKill (void *pvParameters);

void WifiSetup (void *pvParameters);
void WifiIndicator (void *pvParameters);
void WifiServer (void *pvParameters);

void setup () {
  delay(1000);

  xTaskCreatePinnedToCore(
    Blink3,
    "startupBlink",
    1000,
    NULL,
    90,
    &startupRoutine,
    1
  );

  xTaskCreatePinnedToCore(
    SerialSetup,
    "serialSetup",
    1000,
    NULL,
    90,
    &startupRoutine,
    1
  );

  xTaskCreatePinnedToCore(
    WifiSetup,
    "wifiSetup",
    2000,
    NULL,
    80,
    &networkRoutine,
    0
  );

  xTaskCreatePinnedToCore(
    WifiIndicator, // Only runs once.
    "wifiSetup",
    1000,
    NULL,
    80,
    &startupRoutine,
    1
  );
}

void loop () {

}

void Blink3 (void *pvParameters) {
  // setup routine
  pinMode(LED_BUILTIN, OUTPUT);

  // blink
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay((TickType_t) 100 / portTICK_PERIOD_MS);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay((TickType_t) 200 / portTICK_PERIOD_MS);
  }

  // exit routine
  pinMode(LED_BUILTIN, INPUT);

  vTaskDelete(NULL);
}

void SerialSetup (void *pvParameters) {
  Serial.begin(115200);
  vTaskDelay((TickType_t) 500 / portTICK_PERIOD_MS);  
  Serial.println("Serial monitor is ready.");

  xSerialSemaphore = xSemaphoreCreateBinary();
  vTaskDelay((TickType_t) 50000 / portTICK_PERIOD_MS);

  Serial.println("Closing serial monitor.");
  vTaskDelay((TickType_t) 500 / portTICK_PERIOD_MS);
  Serial.end();

  vTaskDelete(NULL);
}

void WifiSetup (void *pvParameters) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) vTaskDelay((TickType_t) 50 / portTICK_PERIOD_MS);

  while (xSerialSemaphore == NULL && xSemaphoreTake(xSerialSemaphore, (TickType_t) 5 / portTICK_PERIOD_MS) != pdTRUE) vTaskDelay((TickType_t) 100 / portTICK_PERIOD_MS);
  Serial.println(WiFi.localIP());
  xWifiSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(xSerialSemaphore);

  xTaskCreatePinnedToCore(
    WifiServer,
    "server",
    1500,
    NULL,
    80,
    NULL,
    1
  );

  vTaskDelete(NULL);
}

void WifiIndicator (void *pvParameters) {
  while (xWifiSemaphore == NULL) vTaskDelay((TickType_t) 50 / portTICK_PERIOD_MS);
  xTaskCreatePinnedToCore(
    Blink3,
    "wifiReadyIndicator",
    1000,
    NULL,
    90,
    &startupRoutine,
    1
  );

  vTaskDelete(NULL);
}

void WifiServer (void *pvParameters) {
  WiFiServer server(80);
  while (xWifiSemaphore == NULL) vTaskDelay((TickType_t) 50 / portTICK_PERIOD_MS);
  server.begin();

  for (;;) {
    WiFiClient client = server.accept();

    if (client) {
      String request;
      while (client.connected()) {

        if (client.available()) {
          String line = client.readStringUntil('\r');

          if (line.startsWith("GET") && line.endsWith("HTTP/1.1") && !line.startsWith("GET /favicon.ico")) { // request decoder
            request = line.substring(line.indexOf("GET"), line.lastIndexOf(" HTTP/1.1"));
          }

          if (line.length() == 1 && line[0] == '\n') { // response manager
            if (request == "GET /") client.println(httpResponse());
            else if (request == "GET /123") client.println(httpResponse());
            break;
          }
        }
      }

      while (client.available()) client.read();
      client.stop();
    }
  }

  vTaskDelete(NULL);
}