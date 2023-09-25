/*
 *Filename: Code Arduino N.U.T. 
 *Author: Jonas Siebel
 *Date:22.09.2023
 *Version: 1.1
 *
 *Password for NUTS-WIFI: 
 *123456789
 *
 *Web-Adress for remote controll:
 *192.168.4.1
*/


/*
UnschÃ¶n:
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
const int irsenspin1 = A1;
const int irsenspin2 = A0;

//pins Radio
const int radiopin = 11;


//=================================global variables==============================

bool operatingmode = true;      //true -> remote controlled, false -> line following
bool inposition = false;        //NUT in position under dispenser in line following mode?
bool automatic_continue = true; //station finished filling, vehicle can start again

int i2c_response;

//local for main() only
bool irstatus1 = 0;
bool irstatus2 = 0;

//speeds for driving curves and driving forward*
int curvespeed = 150;
int forwardspeed = 100;
//*they have to be different, because driving a curve requires more power aka speed


//===============================enumerations for I2C================================
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
  MODE_AUTOMATIC, //activate line following mode
  MODE_MANUEL,    //activate remote controlled driving mode
  POSITION_CHECK,   //in position?
  POSITION_ARRIVED, //yes, in position
  POSITION_NOT_ARRIVED, //no, not in position
  AUTOMATIC_CONTINUE, //switch back in remote controlled mode
  VAR_IRSTATUS1,  //status ir sensor 1
  VAR_IRSTATUS2,  //status ir sensor 2
  VAR_OPMODE, //status operation mode
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
    set_speed(forwardspeed); 
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }

//drive backward
  void move_back(){
    set_speed(forwardspeed); 
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } 

//drive sharp to the right
  void move_sright(){
    set_speed(curvespeed); 
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }

//drive sharp to the left
  void move_sleft(){
    set_speed(curvespeed); 
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } 

//drive to the right
  void move_right(){
    set_speed(curvespeed); 
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }

//drive to the left
  void move_left(){
    set_speed(curvespeed); 
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } 

//=========

//switch radio on/off
  void radio_on(){
    digitalWrite(radiopin, HIGH);
  }

  void radio_off(){
    digitalWrite(radiopin, LOW);
  }

//=======

//communication with esp via I2C (triggerd by an I2C interrupt)
void wireReceiveEvent(int bytes){
  int recieved_command = Wire.read();   //store recieved command
  i2c_response = RESPONSE_OK; 

  Serial.print("i2C Recieved: ");
  Serial.println(recieved_command);

  //interrupts which always work
  switch(recieved_command){
    case VAR_IRSTATUS1:
    i2c_response = irstatus1;
    break;
    
    case VAR_IRSTATUS2:
    i2c_response = irstatus2;
    break;
    
    case VAR_OPMODE:
    i2c_response = operatingmode;
    break;
    
    case RADIO_ON:
    radio_on();
    break;
    
    case RADIO_OFF:
    radio_off();
    break;
  }
   

  //remote controlled driving interrupts
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
  } 
  
  //line following mode interrupts
  else if(operatingmode == 0){
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

//=======
  
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

  pinMode(radiopin, OUTPUT);

  Serial.begin(9600); //start debug

  Wire.begin(1);  //start I2C
  Wire.onReceive(wireReceiveEvent); //interrupt through I2C on
  Wire.onRequest(wireRequestEvent);
}

//==========================================void loop================================
void loop()
{
  

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