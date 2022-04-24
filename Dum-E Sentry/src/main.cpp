#include <Arduino.h>
#include <WebSocketsClient.h>
#include <WiFi.h>

#include "esp_camera.h"
#include "servo.hpp"

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

// Initialize WiFi parameters
const char *ssid = "Device-Northwestern";
const char *passphrase = NULL;

// Instantiate WebSocket client
WebSocketsClient webSocket;

// Initialize camera instance
camera_fb_t *fb = NULL;

//Initializes both servos
Servos servos;

// Timer to take picture every PERIOD milliseconds
unsigned long PERIOD = 1;
unsigned long prevShot;

// Define WebSocket event handler
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            Serial.printf("[WSc] Connected to url: %s\n", payload);
            break;
        case WStype_TEXT:
            servos.moveServos(payload);
            Serial.printf("[WSc] get text: %s\n", payload);
            break;
        case WStype_BIN:
        case WStype_ERROR:
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
            break;
    }
}

void setup() {
    // put your  setup code here, to run once:
    Serial.begin(115200);

    // camera configuration
    camera_config_t config;
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
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 50;
    config.fb_count = 2;

    /*
      // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
      //                      for larger pre-allocated frame buffer.
      if (psramFound()) {
      config.frame_size = FRAMESIZE_UXGA;
      config.jpeg_quality = 10;
      config.fb_count = 2;
      } else {
      config.frame_size = FRAMESIZE_SVGAp;
      config.jpeg_quality = 12;
      config.fb_count = 1;
      }
    */

    // camera initialization
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        esp_camera_fb_return(fb);
        return;
    }

    // Connect to WiFi
    WiFi.begin(ssid, passphrase);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // Connect WebSocket client to WebSocket server
    webSocket.begin("10.105.64.191", 8888, "/sentry");

    // Assign a callback function to the event handler
    webSocket.onEvent(webSocketEvent);

    // Set WebSocket reconnect interval
    webSocket.setReconnectInterval(3000);

    // Initialize first prevShot time
    prevShot = millis();
}

void loop() {
    // put your main code here, to run repeatedly:

    webSocket.loop();

    // if (millis() - prevShot >= PERIOD)
    if (true) {
        // Take picture with camera
        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            esp_camera_fb_return(fb);
            return;
        }

        // Send picture through WebSocket
        webSocket.sendBIN((const uint8_t *)fb->buf, fb->len);
        esp_camera_fb_return(fb);

        // Print FPS as captured from the ESP32
        Serial.printf("%.2f\nlength: %d\n", 1000 / (double)(millis() - prevShot), fb->len);

        // Reset timer
        prevShot = millis();
    }
}