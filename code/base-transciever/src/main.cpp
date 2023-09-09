#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "ESPAsyncWebServer.h"
#include <String.h>
#include <Servo.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

AsyncWebServer server(80);

const char* ssid = "NUT";
const char* password = "123456789";
IPAddress local_IP(192, 168, 4, 200);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

unsigned long previousMillis = 0;
const long interval = 5000; 

String test;
String link = "http://192.168.4.1/link1";

#define VALVE1_PIN 14
#define VALVE2_PIN 12
#define VALVE3_PIN 13
#define VALVE4_PIN 15

#define VALVE_POS_CLOSED 0
#define VALVE_POS_OPEN 180

Servo valve1, valve2, valve3, valve4;

const int TrigSensPin = A0;
bool foundVehicle = false;

bool automaticMode = true;
int automaticState = 0;

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

void controlValves(String valveNum, String state) {
  Servo valve;
  if (strcmp(valveNum.c_str(), "valve1") == 0){
    valve = valve1;
  } else if (strcmp(valveNum.c_str(), "valve2") == 0){
    valve = valve2;
  } else if (strcmp(valveNum.c_str(), "valve3") == 0){
    valve = valve3;
  } else if (strcmp(valveNum.c_str(), "valve4") == 0){
    valve = valve4;
  } else {
    Serial.println("Error parsing Command (valveNum)");
    return;
  }
  
  if (strcmp(state.c_str(), "open") == 0){
    valve.write(VALVE_POS_OPEN);
  } else if (strcmp(state.c_str(), "close") == 0){
    valve.write(VALVE_POS_CLOSED);
  } else {
    Serial.println("Error parsing Command (state)");
    return;
  }
  
}

void setup() {
  Serial.begin(9600);
  Serial.println();

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  } 
 
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi: "+WiFi.localIP().toString());

  server.on("/valve", HTTP_GET, [](AsyncWebServerRequest *request){
    String valveNum;
    String state;
    // GET input1 value on <ESP_IP>/control?command=<inputMessage1>&value=<inputMessage2>
    if (request->hasParam("valveNum") && request->hasParam("state")) {
      valveNum = request->getParam("valveNum")->value();
      state = request->getParam("state")->value();
    }
    else {
      valveNum = "No message sent";
      state = "No message sent";
    }
    Serial.print("valveNum: ");
    Serial.print(valveNum);
    Serial.print(", Value: ");
    Serial.println(state);
    request->send_P(200, "text/plain", "OK");
    controlValves(valveNum, state);
  });

  server.begin();

  valve1.attach(VALVE1_PIN, 900, 1500);
  valve2.attach(VALVE2_PIN, 900, 1500);
  valve3.attach(VALVE3_PIN, 900, 1500);
  valve4.attach(VALVE4_PIN, 900, 1500);

  valve1.write(VALVE_POS_CLOSED);
  valve2.write(VALVE_POS_CLOSED);
  valve3.write(VALVE_POS_CLOSED);
  valve4.write(VALVE_POS_CLOSED);

}

void loop() {
  String result;
  foundVehicle = analogRead(TrigSensPin) < 100;
  
  if (automaticMode) {
    switch (automaticState) {
      case 0: //wait for vehicle to be in range
        if (foundVehicle) {
          automaticState = 10;
        }
        break;

      case 10: //tell vehicle to follow line
        result = httpGETRequest("http://192.168.4.1/control?command=automatic&value=1");
        Serial.println(result);
        automaticState = 20;
        break;

      case 20: //wait until vehicle leaves the sensor
        if (!foundVehicle) {
          automaticState = 0;
        }
        break;

    } 
  } else {
    automaticState = 0;
  }
  /*
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval) {
     // Check WiFi connection status
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      test = httpGETRequest(link.c_str());
      Serial.println(test);
      
      
      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }*/
}

