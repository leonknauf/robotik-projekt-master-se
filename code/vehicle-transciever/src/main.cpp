#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "ESPAsyncWebServer.h"
#include "webpage.h"
#include <Wire.h>

const char* ssid = "NUT";
const char* password = "123456789";

unsigned long previousMillis = 0;
const long interval = 5000; 

AsyncWebServer server(80);

String test;

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path
  http.begin(client, serverName);
  
  // Send HTTP POST request
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

void sendOverWire(int value) {
  Wire.beginTransmission(1); // transmit to device #1
  Wire.write(value);              // sends x 
  Wire.endTransmission();    // stop transmitting
}

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.print("Setting AP (Access Point)…");

  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", "OK");
    Serial.println("stop");
    sendOverWire(0);
  });
  server.on("/forward", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", "OK");
    Serial.println("forward");
    sendOverWire(1);
  });
  server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", "OK");
    Serial.println("left");
    sendOverWire(2);
  });
  server.on("/right", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", "OK");
    Serial.println("right");
    sendOverWire(3);
  });
  server.on("/continue", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", "OK");
    Serial.println("continue");
    sendOverWire(4);
  });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", MAIN_page);
    Serial.println("root");
  });

  // Start server
  server.begin();

  Wire.begin(); 
}

void loop() {
  /*
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval) {
    // Check WiFi connection status
    test = httpGETRequest("http://192.168.4.2/link2");
    Serial.println(test);
      
      
    // save the last HTTP GET Request
    previousMillis = currentMillis;

  }*/

}
