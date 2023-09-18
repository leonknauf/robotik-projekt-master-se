#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "ESPAsyncWebServer.h"
#include <String.h>
#include <Servo.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

AsyncWebServer server(80);

const char* ssid = "NUTS";
const char* password = "123456789";
IPAddress local_IP(192, 168, 4, 200);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

unsigned long previousMillis = 0;
const long interval = 5000; 
int numConfiguredValves = 1;

String test;
String link = "http://192.168.4.1/link1";

#define VALVE1_PIN 14
#define VALVE2_PIN 12
#define VALVE3_PIN 13
#define VALVE4_PIN 15

#define VALVE_POS_CLOSED 0
#define VALVE_POS_OPEN 180

#define VALVE_OPEN_TIME 500 //ms

Servo valve1, valve2, valve3, valve4, currentValve;
int currentValveNum;

bool startAutomaticValveControl;
bool automaticDone;

const int TrigSensPin = A0;
bool foundVehicle = false;

bool enableAutomaticMode = true;
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
  int iValveNum = valveNum.toInt();
  int iState = state.toInt();

  switch (iValveNum) {
    case 1:
      currentValve = valve1;
      break;
    
    case 2:
      currentValve = valve2;
      break;
    
    case 3:
      currentValve = valve3;
      break;

    case 4:
      currentValve = valve4;
      break;

    default:
      Serial.println("Error parsing Command (valveNum)");
      return;
  }
  currentValveNum = iValveNum;

  switch (iState) {
    case 0: //Manual close
      Serial.print("Manual valve control: close valve");
      Serial.println(iValveNum);
      currentValve.write(VALVE_POS_CLOSED);
      break;

    case 1: //Manual open
      Serial.print("Manual valve control: open valve");
      Serial.println(iValveNum);
      currentValve.write(VALVE_POS_OPEN);
      break;

    case 2: //Automatic open and close
      startAutomaticValveControl = true;
      if (iValveNum >= numConfiguredValves) {
        automaticDone = true;
      }
      break;

    default:
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

  if (numConfiguredValves >= 1) {
    valve1.attach(VALVE1_PIN, 900, 1500);
    valve1.write(VALVE_POS_CLOSED);
  }
  if (numConfiguredValves >= 2) {
    valve2.attach(VALVE2_PIN, 900, 1500);
    valve2.write(VALVE_POS_CLOSED);
  }
  if (numConfiguredValves >= 3) {
    valve3.attach(VALVE3_PIN, 900, 1500);
    valve3.write(VALVE_POS_CLOSED);
  }
  if (numConfiguredValves >= 4) {
    valve4.attach(VALVE4_PIN, 900, 1500);
    valve4.write(VALVE_POS_CLOSED);
  }
}

void loop() {
  String response;
  foundVehicle = analogRead(TrigSensPin) < 100;
  
  if (enableAutomaticMode) {
    switch (automaticState) {
      case 0: //wait for vehicle to be in range
        startAutomaticValveControl = false;
        automaticDone = false;

        if (foundVehicle) { //tell vehicle to follow line
          response = httpGETRequest("http://192.168.4.1/control?command=automatic&value=1");
          Serial.print("Sending automatic=1 request, response: ");
          Serial.println(response);
          automaticState = 10;
        }
        break;

      case 10: 
        if (startAutomaticValveControl) { //Control the current valve
          Serial.print("Automatic valve control: open valve");
          Serial.println(currentValveNum);
          currentValve.write(VALVE_POS_OPEN);
          previousMillis = millis();
          startAutomaticValveControl = false;
          automaticState = 20;
        }
        break;

      case 20:
        if (millis() - previousMillis >= VALVE_OPEN_TIME) {
          Serial.print("Automatic valve control: close valve");
          Serial.println(currentValveNum);
          currentValve.write(VALVE_POS_CLOSED);
          if (automaticDone){
            response = httpGETRequest("http://192.168.4.1/control?command=automatic&value=0");
            Serial.print("Sending automatic=0 request, response: ");
            Serial.println(response);
            automaticState = 0;
          } else {
            response = httpGETRequest("http://192.168.4.1/control?command=continue&value=1");
            Serial.print("Sending continue=1 request, response: ");
            Serial.println(response);
            automaticState = 10;
          }
        }
        break;

      default:
        automaticState = 0;
        break;
    } 
  } else {
    automaticState = 0;
  }
}

