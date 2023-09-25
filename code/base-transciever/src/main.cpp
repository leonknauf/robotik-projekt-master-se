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
unsigned long readAnalogMillis = 0;
const long interval = 5000; 
int numConfiguredValves = 4;

String test;
String link = "http://192.168.4.1/link1";

#define VALVE1_PIN 15
#define VALVE2_PIN 13
#define VALVE3_PIN 12
#define VALVE4_PIN 14

#define VALVE_POS_CLOSED 0
#define VALVE_POS_OPEN 180

#define VALVE_OPEN_TIME 100 //ms
#define READ_ANALOG_INTERVAL 100

Servo valve1, valve2, valve3, valve4, currentValve;
int currentValveNum;

bool AutomaticValveControl;
bool AutomaticValveControl_mem;

const int TrigSensPin = A0;
bool foundVehicle = false;

bool enableAutomaticMode;
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

  if (enableAutomaticMode and (iValveNum > 0 or iState != 2))
    return;

  switch (iValveNum) {
    case 0:
      iValveNum = currentValveNum+1;

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
      AutomaticValveControl = true;
      break;

    default:
      Serial.println("Error parsing Command (state)");
      return;
  }

}

String readWriteValue(String valueName, String value) {
  if (strcmp(value.c_str(), "request") == 0) {
    //Read value

    if (strcmp(valueName.c_str(), "enableAutomaticMode") == 0)
      return enableAutomaticMode ? "true" : "false";
    else if (strcmp(valueName.c_str(), "automaticState") == 0)
      return String(automaticState);
    else if (strcmp(valueName.c_str(), "numConfiguredValves") == 0)
      return String(numConfiguredValves);
    else if (strcmp(valueName.c_str(), "foundVehicle") == 0) {
      Serial.print("foundVehicle");
      Serial.println(foundVehicle);
      return foundVehicle ? "true" : "false";
    } else
      return "Error";

  } else {
    //Write value

    if (strcmp(valueName.c_str(), "enableAutomaticMode") == 0) {
      if (strcmp(value.c_str(), "true") == 0)
        enableAutomaticMode = true;
      else if (strcmp(value.c_str(), "false") == 0 and automaticState == 0)
        enableAutomaticMode = false;
      return enableAutomaticMode ? "true" : "false";
    } else if (strcmp(valueName.c_str(), "numConfiguredValves") == 0) {
      if (value.toInt() > 0 and value.toInt() <= 4)
        numConfiguredValves = value.toInt();
      return String(numConfiguredValves);
    } else
      return "Error";
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

  server.on("/value", HTTP_GET, [](AsyncWebServerRequest *request){
    String valueName, value;
    // GET input1 value on <ESP_IP>/value?valueName=<inputMessage1>&value=<inputMessage2>
    if (request->hasParam("valueName")) {
      valueName = request->getParam("valueName")->value();
    }
    else {
      valueName = "No message sent";
    }
    if (request->hasParam("value")) {
      value = request->getParam("value")->value();
    }
    else {
      value = "request";
    }

    Serial.print("valueName: ");
    Serial.print(valueName);
    Serial.print(", value: ");
    Serial.println(value);
    
    String response = readWriteValue(valueName, value);
    AsyncWebServerResponse *httpresponse = request->beginResponse(200, "text/plain", response.c_str());
    //Serial.print("response: ");
    //Serial.println(response);
    httpresponse->addHeader("Access-Control-Allow-Headers", "*");
    httpresponse->addHeader("Access-Control-Allow-Origin", "*");
    httpresponse->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
    request->send(httpresponse);
  });

  server.begin();

  valve1.attach(VALVE1_PIN, 900, 1500);
  valve1.write(VALVE_POS_CLOSED);
  valve2.attach(VALVE2_PIN, 900, 1500);
  valve2.write(VALVE_POS_CLOSED);
  valve3.attach(VALVE3_PIN, 900, 1500);
  valve3.write(VALVE_POS_CLOSED);
  valve4.attach(VALVE4_PIN, 900, 1500);
  valve4.write(VALVE_POS_CLOSED);

  Serial.println("Setup done");
}

void loop() {
  String response;
  if (millis() - readAnalogMillis >= READ_ANALOG_INTERVAL) {
    foundVehicle = analogRead(TrigSensPin) > 600;
    readAnalogMillis = millis();
  }
  
  
  if (enableAutomaticMode) {
    switch (automaticState) {
      case 0: //wait for vehicle to be in range
        AutomaticValveControl = false;
        currentValveNum = 0;

        if (foundVehicle) { //tell vehicle to follow line
          response = httpGETRequest("http://192.168.4.1/control?command=automatic&value=1");
          Serial.print("Sending automatic=1 request, response: ");
          Serial.println(response);
          automaticState = 10;
        }
        break;

      case 10: 
        if (AutomaticValveControl) {
          Serial.println("Filling vehicle");
          automaticState = 20;
        }
        break;

      case 20:
        if (!AutomaticValveControl) {
          if (currentValveNum >= numConfiguredValves) {
            Serial.println("vehicle filled, done");
            response = httpGETRequest("http://192.168.4.1/control?command=automatic&value=0");
            Serial.print("Sending automatic=0 request, response: ");
            Serial.println(response);
            automaticState = 0;
          } else {
            Serial.println("vehicle filled, continue");
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

  if (AutomaticValveControl) { //Control the current valve
    if (!AutomaticValveControl_mem) {
      Serial.print("Automatic valve control: open valve");
      Serial.println(currentValveNum);
      currentValve.write(VALVE_POS_OPEN);
      previousMillis = millis();
      AutomaticValveControl_mem = true;
    } else {
      if (millis() - previousMillis >= VALVE_OPEN_TIME) {
          Serial.print("Automatic valve control: close valve");
          Serial.println(currentValveNum);
          currentValve.write(VALVE_POS_CLOSED);   
          AutomaticValveControl = false;
          AutomaticValveControl_mem = false; 
      }
    }
  } 
}

