#include <Arduino.h>
#include "esp_camera.h"
#include "board_config.h"

static bool isRedPixel(uint8_t r, uint8_t g, uint8_t b) {
  // Starting threshold for a bright red object.
  // We will tune this after seeing the results.
  return (r > 180) &&
         (r > g + 50) &&
         (r > b + 50);
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  delay(1500);

  Serial.println();
  Serial.println("Starting red-car detection test...");

  camera_config_t config = {};

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;

  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;

  // RGB888 gives direct R, G, B values for each pixel.
  config.pixel_format = PIXFORMAT_RGB565;
  config.frame_size = FRAMESIZE_QQVGA;  // 160 × 120

  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.fb_count = 1;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Camera initialization failed: 0x%x\n", err);
    while (true) {
      delay(1000);
    }
  }

  sensor_t* sensor = esp_camera_sensor_get();

  if (sensor != nullptr) {
    sensor->set_brightness(sensor, 0);
    sensor->set_contrast(sensor, 0);
    sensor->set_saturation(sensor, 0);
    sensor->set_sharpness(sensor, 0);

#if defined(CAMERA_MODEL_ESP32S3_EYE)
    sensor->set_vflip(sensor, 1);
#endif
  }

  Serial.println("Camera initialized successfully.");
}

void loop() {
  camera_fb_t* frame = esp_camera_fb_get();

  if (frame == nullptr) {
    Serial.println("Camera capture failed.");
    delay(500);
    return;
  }

  const int width = frame->width;
  const int height = frame->height;

  long xSum = 0;
  long ySum = 0;
  int redPixelCount = 0;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int index = (y * width + x) * 2;

      // RGB565 pixel stored using two bytes
      uint16_t pixel =
          ((uint16_t)frame->buf[index] << 8) |
          frame->buf[index + 1];

      // Extract RGB565 channels
      uint8_t r5 = (pixel >> 11) & 0x1F;
      uint8_t g6 = (pixel >> 5) & 0x3F;
      uint8_t b5 = pixel & 0x1F;

      // Convert approximately to 0–255
      uint8_t r = (r5 * 255) / 31;
      uint8_t g = (g6 * 255) / 63;
      uint8_t b = (b5 * 255) / 31;

      if (isRedPixel(r, g, b)) {
        xSum += x;
        ySum += y;
        redPixelCount++;
      }
    }
  }

  if (redPixelCount > 100) {
    int centerX = xSum / redPixelCount;
    int centerY = ySum / redPixelCount;

    int detectedZone;

    if (centerX < width / 3) {
      detectedZone = 1;
    } else if (centerX < (2 * width) / 3) {
      detectedZone = 2;
    } else {
      detectedZone = 3;
    }

    Serial.print("Red pixels: ");
    Serial.print(redPixelCount);

    Serial.print(" | Center X: ");
    Serial.print(centerX);

    Serial.print(" | Center Y: ");
    Serial.print(centerY);

    Serial.print(" | Zone: ");
    Serial.println(detectedZone);
  } else {
    Serial.print("No car detected | Red pixels: ");
    Serial.println(redPixelCount);
  }

  esp_camera_fb_return(frame);

  delay(250);
}