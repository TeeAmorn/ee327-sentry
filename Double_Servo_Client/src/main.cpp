#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "servo.hpp"

const char* ssid = "Amongus";
const char* password = NULL;


unsigned long previousMillis = 0;
const long interval = 5000;
unsigned long currentMillis;
Servos servos;

String direction;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting...");
    while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  servos.moveServos("N"); //just want to initialize the position of the servos
}

void loop() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    //check wifi connections status
    if (WiFi.status() == WL_CONNECTED) {
      //input code to request from the sever
      direction = httpGETRequest(); //input server name
      servos.moveServos(direction);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);

  //// Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "--"; 

  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}