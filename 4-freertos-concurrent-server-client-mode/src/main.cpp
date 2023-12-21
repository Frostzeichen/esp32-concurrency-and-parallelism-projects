#include <Arduino.h>
#include <WiFi.h>

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
    WifiIndicator,
    "wifiSetup",
    1000,
    NULL,
    80,
    &networkRoutine, // If running on stealth mode, set to startupRoutine so it won't blink after setup period.
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