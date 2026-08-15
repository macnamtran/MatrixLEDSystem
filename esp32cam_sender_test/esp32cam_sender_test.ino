#include <WiFi.h>
#include <esp_now.h>

uint8_t receiverMac[] = {0xAC, 0x15, 0x18, 0xD5, 0x68, 0x70};

typedef struct DetectionData {
  bool carDetected;
  int zone; // 0 = none, 1 = left, 2 = center, 3 = right
} DetectionData;

DetectionData dataToSend;

void sendDetection(bool carDetected, int zone) {
  dataToSend.carDetected = carDetected;
  dataToSend.zone = zone;

  esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&dataToSend, sizeof(dataToSend));

  Serial.print("Sent -> carDetected: ");
  Serial.print(carDetected);
  Serial.print(" | zone: ");
  Serial.print(zone);
  Serial.print(" | result: ");

  if (result == ESP_OK) {
    Serial.println("ESP_OK");
  } else {
    Serial.print("ERROR: ");
    Serial.println(result);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  delay(500);

  Serial.print("CAM Sender MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add receiver peer");
    return;
  }

  Serial.println("ESP32-CAM sender ready");
}

void loop() {
  sendDetection(true, 1);   // car left
  delay(2000);

  sendDetection(true, 2);   // car center
  delay(2000);

  sendDetection(true, 3);   // car right
  delay(2000);

  sendDetection(false, 0);  // no car
  delay(2000);
}