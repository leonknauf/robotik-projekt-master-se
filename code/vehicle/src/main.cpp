/*
 *Filename: Code Arduino N.U.T. 
 *Author: Jonas Siebel
 *Date: 09.09.2023
 *Version: 1.0
*/


/*
Unschön:
1) Es gibt globale variablen
2) Die Funktion die bei einem I2C INterrupt aufgerufen wird ist zu lang
   Vielleicht mit Markern arbeiten und einen Teil davon ins Hauptprogramm verschieben?
*/


//==================================libs=============================

#include <Servo.h>    //servo motor lib
#include <HCSR04.h>   //ultrasonic lib
#include <Wire.h>     //I2C lib

//==================================pins==============================

//pins motor a
int enA = 9;
int in1 = 8;
int in2 = 7;

//pins motor b
int enB = 3;
int in3 = 5;
int in4 = 4;

//pins ultrasonic sensor
const int trigPin = 13;
const int echoPin = 12;
UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);

//pins IR-Sensors
const int irsenspin1 = A0;
const int irsenspin2 = A1;


//=================================global variables==============================

bool operatingmode = true;    //true -> remote controlled, false -> line following
bool inposition = false;      //NUT in position under dispenser in line following mode?

int i2c_response;

//local for main() only
bool irstatus1 = 0;
bool irstatus2 = 0;

//==================================functions====================================

//set speed of all motors on a value between 0 and 255 (min. 100 recommended)
void set_speed(int velocity){
  analogWrite(enA, velocity);
  analogWrite(enB, velocity);
}

//stop the movement
  void move_stop(){
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }

//drive forward
  void move_for(){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }

//drive backward
  void move_back(){
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } 

//drive sharp to the right
  void move_sright(){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }

//drive sharp to the left
  void move_sleft(){
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } 

//drive to the right
  void move_right(){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }

//drive to the left
  void move_left(){
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } 


//communication with esp via I2C (triggerd by an I2C interrupt)
void wireReceiveEvent(int bytes){
  int recieved_command = Wire.read();
  i2c_response = 1;

  Serial.print("i2C Recieved: ");
  Serial.println(recieved_command);
  //remote controlled driving interrupt
  if(operatingmode == 1){
    switch(recieved_command){
      case 10: case 20: case 30: case 40: case 50: case 60:
      move_stop();
      break;
  
      case 11:
      move_for();
      break;
  
      case 21:
      move_back();
      break;
  
      case 31:
      move_left();
      break;
  
      case 41:
      move_right();
      break;
  
      case 51:
      move_sleft();
      break;
  
      case 61:
      move_sright();
      break;

      //switch to line following mode
      case 200:
      operatingmode = 0;
      break;
    }
  } else if(operatingmode == 0){
    switch (recieved_command){    
      case 69://are you in position?
        if(inposition == true){
          i2c_response = 77; //yes, in position
          inposition = false;
        }
        else{
          i2c_response = 88; //no, not in position
        }
        break;

      //switch to rc driving mode
      case 100:
        operatingmode = 1;
        break;
    }
  }
}
  
void wireRequestEvent(){
  Serial.print("i2C Request, answering: ");
  Serial.println(i2c_response);
  Wire.write(i2c_response);
  i2c_response = 1;
}

//=========================================setup================================

void setup()
{
  //Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  Serial.begin(9600); //start debug

  Wire.begin(1);  //start I2C
  Wire.onReceive(wireReceiveEvent); //interrupt through I2C on
  Wire.onRequest(wireRequestEvent);
}

//==========================================void loop================================
void loop()
{
  set_speed(100); //set speed at 100

  //after here is a state machine with only two states: remote mode or line following mode

  //line following inside station loop
  if(operatingmode == 0){ 
    irstatus1 = digitalRead(irsenspin1);
    irstatus2 = digitalRead(irsenspin2);

    //both see white = line is in between -> all good
    if(irstatus1 == true && irstatus2 == true){
      move_for();
      //delay(50);
    }

    //ir1 sees black = line is left to the left
    if(irstatus1 == false && irstatus2 == true){
      move_right();
      //delay(50);
    }

    //ir2 sees black = line is left to the right
    if(irstatus1 == true && irstatus2 == false){
      move_left();
      //delay(50);
    }

    //both see black = a stop point is reached
    if(irstatus1 == false && irstatus2 == false){
      move_stop();
      inposition = true;  //set flag to mark that position is reached
      //delay(50);
    }



    /*
    //wenn Linie nicht gefunden "schnüffle" nach links und rechts
    else{
      if (currentMillis - previousMillis >= 1000) {
        //save the last time you blinked the LED
        previousMillis = currentMillis;
        if(toggle == 1){
          toggle = 0;
          move_sleft();
        }
        else{
          toggle = 1;
          move_sright();
        }
      }
      
    }*/

    
  }

  //remote controlled driving loop
  if(operatingmode == 1){  
    //empty. everything is done in interrupt
  }

  

  /*
  //debugging
  Serial.print("Steering: ");
  Serial.print(steering);
  Serial.print(", drive: ");
  Serial.print(drive);
  Serial.print(", linePosition: ");
  Serial.print(linePosition);
  Serial.print(", foundLine: ");
  Serial.print(foundLine);
  Serial.print(", memFoundLine: ");
  Serial.print(memFoundLine);
  Serial.print(", startTimeSearch: ");
  Serial.print(startTimeSearch);
  Serial.print(", millis: ");
  Serial.print(millis());
  */
}