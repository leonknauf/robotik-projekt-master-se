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

bool operatingmode = true;      //true -> remote controlled, false -> line following
bool inposition = false;        //NUT in position under dispenser in line following mode?
bool automatic_continue = true; //station finished filling, vehicle can start again

int i2c_response;

//local for main() only
bool irstatus1 = 0;
bool irstatus2 = 0;


//=================================enumerations==================================
enum i2cCommand{
  UNDEFINED,
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
  AUTOMATIC_CONTINUE, 
  VAR_IRSTATUS1,
  VAR_IRSTATUS2,
  VAR_OPMODE,
  RADIO_ON,
  RADIO_OFF
};


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
  i2c_response = RESPONSE_OK; 


  Serial.print("i2C Recieved: ");
  Serial.println(recieved_command);

  if(recieved_command == VAR_IRSTATUS1){
     i2c_response = irstatus1;
  } else if(recieved_command == VAR_IRSTATUS2){
     i2c_response = irstatus2;
  } else if(recieved_command == VAR_OPMODE){
     i2c_response = operatingmode;
  } else if(recieved_command == RADIO_ON) {
    //Turn on Radio
  } else if(recieved_command == RADIO_OFF) {
    //Turn off Radio
  }
  

 

  //remote controlled driving interrupt
  if(operatingmode == 1){
    switch(recieved_command){
      case MOVE_STOP:
      move_stop();
      break;
  
      case MOVE_FORWARD:
      move_for();
      break;
  
      case MOVE_BACKWARD:
      move_back();
      break;
  
      case MOVE_LEFT:
      move_left();
      break;
  
      case MOVE_RIGHT:
      move_right();
      break;
  
      case MOVE_HARD_LEFT:
      move_sleft();
      break;
  
      case MOVE_HARD_RIGHT:
      move_sright();
      break;

      //switch to line following mode
      case MODE_AUTOMATIC:
      operatingmode = 0;
      break;

    }
  } else if(operatingmode == 0){
    switch (recieved_command){    
      case POSITION_CHECK://are you in position?
        if(inposition == true){
          i2c_response = POSITION_ARRIVED; //yes, in position
          inposition = false;
        }
        else{
          i2c_response = POSITION_NOT_ARRIVED; //no, not in position
        }
        break;

      //switch to rc driving mode
      case MODE_MANUEL:
        operatingmode = 1;
        break;

      //station complete, continue
      case AUTOMATIC_CONTINUE:
        automatic_continue = true;
        break;
    }
  }
}
  
void wireRequestEvent(){
  Serial.print("i2C Request, answering: ");
  Serial.println(i2c_response);
  Wire.write(i2c_response);
  i2c_response = RESPONSE_OK;
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
      if (automatic_continue) { //station finished, move forward
        move_for();
      } else { // stop at station
        move_stop(); 
        inposition = true;  //set flag to mark that position is reached
      }
      //delay(50);
    }

    //vehicle left station, reset flag
    if ((irstatus1 == true || irstatus2 == true) && automatic_continue) {
      automatic_continue = false;
    }

    
  }

  //remote controlled driving loop
  if(operatingmode == 1){  
    //empty. everything is done in interrupt
  }

}