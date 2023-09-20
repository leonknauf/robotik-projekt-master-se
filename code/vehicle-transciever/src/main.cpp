#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "ESPAsyncWebServer.h"
#include "webpage.h"
#include <Wire.h>

const char* ssid = "NUTS";
const char* password = "123456789";

unsigned long previousMillis = 0;
const long checkInterval = 500; 

bool checkForVehiclePosition = false;

AsyncWebServer server(80);

String test;

enum i2cCommand{
  UNDEFINDED,
  RESPONSE_OK,
  MOVE_STOP,
  MOVE_FORWARD,
  MOVE_BACKWARD,
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_HARD_LEFT,
  MOVE_HARD_RIGHT,
  MODE_AUTOMATIC,
  MODE_MANUEL,
  POSITION_CHECK,
  POSITION_ARRIVED,
  POSITION_NOT_ARRIVED,
  AUTOMATIC_CONTINUE
};

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
  int command_to_vehicle = UNDEFINDED;
  if (value.toInt() < 0) {
    Serial.println("Error parsing Command (value)");
    return;
  }

  if (strcmp(command.c_str(), "forward") == 0){
    command_to_vehicle = (value.toInt() > 0) ? MOVE_FORWARD : MOVE_STOP;
  } else if (strcmp(command.c_str(), "backward") == 0){
    command_to_vehicle = (value.toInt() > 0) ? MOVE_BACKWARD : MOVE_STOP;
  } else if (strcmp(command.c_str(), "left") == 0){
    command_to_vehicle = (value.toInt() > 0) ? MOVE_LEFT : MOVE_STOP;
  } else if (strcmp(command.c_str(), "right") == 0){
    command_to_vehicle = (value.toInt() > 0) ? MOVE_RIGHT : MOVE_STOP;
  } else if (strcmp(command.c_str(), "hard_left") == 0){
    command_to_vehicle = (value.toInt() > 0) ? MOVE_HARD_LEFT : MOVE_STOP;
  } else if (strcmp(command.c_str(), "hard_right") == 0){
    command_to_vehicle = (value.toInt() > 0) ? MOVE_HARD_RIGHT : MOVE_STOP;
  } else if (strcmp(command.c_str(), "automatic") == 0){
    command_to_vehicle = (value.toInt() > 0) ? MODE_AUTOMATIC : MODE_MANUEL;

    checkForVehiclePosition = true;
  } else if (strcmp(command.c_str(), "continue") == 0) {
    if (value.toInt() > 0) {
      command_to_vehicle = AUTOMATIC_CONTINUE;
    }
  } else {
    Serial.println("Error parsing Command (command)");
  }

  sendOverWire(command_to_vehicle);
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
  String response;
  if (checkForVehiclePosition and (millis() - previousMillis >= checkInterval)) {
    Serial.println("");
    Serial.print("Checking fort vehicle positon... ");
    byte result = sendOverWire(POSITION_CHECK);
    
    if (result == POSITION_NOT_ARRIVED) {
      Serial.println("Not reached the goal yet");
    } else if (result == POSITION_ARRIVED) {
      Serial.println("Vehicle reached the goal");
      response = httpGETRequest("http://192.168.4.200/valve?valveNum=0&state=2");
      Serial.print("Sending valveNum=0 state=2 request, response: ");
      Serial.println(response);
    } else {
      Serial.println("Unknown Response");
    }
    previousMillis = millis();
  }

}
