#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "ESPAsyncWebServer.h"
#include "webpage.h"
#include <Wire.h>

const char* ssid = "NUT";
const char* password = "123456789";

unsigned long previousMillis = 0;
const long checkInterval = 500; 

bool checkForVehiclePosition = false;

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

byte sendOverWire(int value) {
  Wire.beginTransmission(1); // transmit to device #1
  Wire.write(value);              // sends x 
  Wire.endTransmission();    // stop transmitting
  Serial.print("Sending ");
  Serial.print(value);
  Serial.println(" to slave 1");

  Wire.requestFrom(1,1); //address 1, bytes 1
  byte response = Wire.read();
  Serial.print("Recieved response: ");
  Serial.println(response);
  return response;
}

void parseCommand(String command, String value) {
  int command_to_vehicle = 0;
  if (strcmp(command.c_str(), "forward") == 0){
    command_to_vehicle = 10;
  } else if (strcmp(command.c_str(), "backward") == 0){
    command_to_vehicle = 20;
  } else if (strcmp(command.c_str(), "left") == 0){
    command_to_vehicle = 30;
  } else if (strcmp(command.c_str(), "right") == 0){
    command_to_vehicle = 40;
  } else if (strcmp(command.c_str(), "hard_left") == 0){
    command_to_vehicle = 50;
  } else if (strcmp(command.c_str(), "hard_right") == 0){
    command_to_vehicle = 60;
  } else if (strcmp(command.c_str(), "velocity") == 0){
    command_to_vehicle = 1000;
  } else if (strcmp(command.c_str(), "automatic") == 0){
    if (value.toInt() > 0) {
      command_to_vehicle = 199;
    } else {
      command_to_vehicle = 100;
    }

    checkForVehiclePosition = true;
  }

  if (command_to_vehicle > 0 and value.toInt() >=0 and value.toInt() <= 255) {
    command_to_vehicle += value.toInt();
    sendOverWire(command_to_vehicle);
  } else{
    Serial.println("Error parsing Command");
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.print("Setting AP (Access Point)…");

  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request){
    String command;
    String value;
    // GET input1 value on <ESP_IP>/control?command=<inputMessage1>&value=<inputMessage2>
    if (request->hasParam("command") && request->hasParam("value")) {
      command = request->getParam("command")->value();
      value = request->getParam("value")->value();
    }
    else {
      command = "No message sent";
      value = "No message sent";
    }
    Serial.print("Command: ");
    Serial.print(command);
    Serial.print(", Value: ");
    Serial.println(value);
    request->send_P(200, "text/plain", "OK");
    parseCommand(command, value);
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

  if (checkForVehiclePosition and (millis() - previousMillis >= checkInterval)) {
    Serial.println("");
    byte result = sendOverWire(69);
    
    if (result == 88) {
      Serial.println("Not reached the goal yet");
    } else if (result == 77) {
      Serial.println("Vehicle reached the goal");
    }
    previousMillis = millis();
  }

}
