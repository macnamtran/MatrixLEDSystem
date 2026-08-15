#include <WiFi.h>
#include <esp_now.h>
#include "esp_mac.h"

const int leftLED = 25;
const int centerLED = 26;
const int rightLED = 27;

typedef struct DetectionData {
  bool carDetected;
  int zone; // 0 = none, 1 = left, 2 = center, 3 = right
} DetectionData;

DetectionData receivedData;

void allLightsOn() {
  digitalWrite(leftLED, HIGH);
  digitalWrite(centerLED, HIGH);
  digitalWrite(rightLED, HIGH);
}

void applyHeadlightLogic(bool carDetected, int zone) {
  allLightsOn();

  if (!carDetected) {
    return;
  }

  if (zone == 1) {
    digitalWrite(leftLED, LOW);
  } else if (zone == 2) {
    digitalWrite(centerLED, LOW);
  } else if (zone == 3) {
    digitalWrite(rightLED, LOW);
  }
}

void printMacAddress() {
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA);

  Serial.print("Receiver STA MAC: ");
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}

void onDataReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  Serial.print("Received -> carDetected: ");
  Serial.print(receivedData.carDetected);
  Serial.print(" | zone: ");
  Serial.println(receivedData.zone);

  applyHeadlightLogic(receivedData.carDetected, receivedData.zone);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(leftLED, OUTPUT);
  pinMode(centerLED, OUTPUT);
  pinMode(rightLED, OUTPUT);

  allLightsOn();

  WiFi.mode(WIFI_STA);
  delay(500);

  printMacAddress();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onDataReceive);

  Serial.println("Normal ESP32 receiver + LED controller ready");
}

void loop() {
}