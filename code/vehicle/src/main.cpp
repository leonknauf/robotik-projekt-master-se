#include <Servo.h>
#include <HCSR04.h>

#define SERVO_SIGNAL_PIN 10

Servo servoMotor;

// Motor A

int enA = 9;
int in1 = 8;
int in2 = 7;

// Motor B

int enB = 3;
int in3 = 5;
int in4 = 4;

// Pins of HC-Sc04 sensor
const int trigPin = 13;
const int echoPin = 12;
UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);

const int TrigSensPin = A0;
int val = 0;

void setup()
{
  
  servoMotor.attach(SERVO_SIGNAL_PIN, 900, 1500);

  // Set all the motor control pins to outputs

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  Serial.begin(9600);
}
/*
void demoOne()

{

  // This function will run the motors in both directions at a fixed speed

  // Turn on motor A

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  // Set speed to 200 out of possible range 0~255

  analogWrite(enA, 255);

  // Turn on motor B

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  // Set speed to 200 out of possible range 0~255

  analogWrite(enB, 255);

  delay(2000);

  // Now change motor directions

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  delay(2000);

  // Now turn off motors

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

}

void demoTwo()

{

  // This function will run the motors across the range of possible speeds
  // Note that maximum speed is determined by the motor itself and the operating voltage

  // Turn on motors

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  // Accelerate from zero to maximum speed

  for (int i = 100; i < 256; i++)

  {

    analogWrite(enA, i);
    analogWrite(enB, i);

    delay(20);

  }

  // Decelerate from maximum speed to zero

  for (int i = 255; i >= 100; --i)

  {

    analogWrite(enA, i);
    analogWrite(enB, i);

    delay(20);

  }

  // Now turn off motors

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

}*/
const int steerSpeed = 100;

int linePosition; //0: unknown, 1: left, 2: right
bool followLine = true;
int steering; //0: straight, 2: left, 1: right
int drive;//0:off, 1: foward, 2: backward
int outA, outB;

bool foundLine;
bool memFoundLine;

unsigned long startTimeSearch;

void movements()
{
  if (drive > 0) {
    outA = 10;
    outB = 10;
    if (steering = 1) {
      outB += steerSpeed;
    } else if (steering = 2) {
      outB += steerSpeed;
    }
  } else {
    if (steering > 0) {
      outA = steerSpeed;
      outB = steerSpeed;
    } else {
      outA = 0;
      outB = 0;
    }
  }
  

  switch (drive)
  {
    case 0:
      switch (steering)
      {
        case 0:
          digitalWrite(in1, LOW);
          digitalWrite(in2, LOW);
          digitalWrite(in3, LOW);
          digitalWrite(in4, LOW);
          break;

        case 1:
          digitalWrite(in1, LOW);
          digitalWrite(in2, HIGH);
          digitalWrite(in3, HIGH);
          digitalWrite(in4, LOW);
          break;

         case 2:
          digitalWrite(in1, HIGH);
          digitalWrite(in2, LOW);
          digitalWrite(in3, LOW);
          digitalWrite(in4, HIGH);
          break;
          
      }
      break;

    case 1:
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      break;

    case 2:
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      break;
  }
  if (outA > 255) {
    outA = 255;
  }
  if (outB > 255) {
    outB = 255;
  }
  analogWrite(enA, outA);
  analogWrite(enB, outB);
}

void loop()
{
  foundLine = analogRead(TrigSensPin) < 100;


  if (followLine) {
    if (linePosition == 0) { //unknown;
      drive = 0;
      if (foundLine) {
        memFoundLine = foundLine;
        if (steering == 0) {
          steering = 1;
        }
      } else {
        if (memFoundLine) {
          linePosition = steering;
          steering = 0;
        } else {
          if (steering == 0){
            steering = 1;
          }
          if (startTimeSearch == 0) {
            startTimeSearch = millis();
          } else {
            if (millis() - startTimeSearch > 2000) {
              if (steering == 1) {
                steering = 2;
              } else if (steering == 2){
                steering = 1;
              }
              startTimeSearch = 0;
            }
          }
        }
      }
    } else {
      steering = 0;
      drive = 1;
    }
  }

  movements();
  //Serial.println(distanceSensor.measureDistanceCm()+1);
  delay(50);
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
  Serial.print(", outA: ");
  Serial.print(outA);
  Serial.print(", outB: ");
  Serial.println(outB);

  //val = analogRead(TrigSensPin);  // read the input pin
  //Serial.println(val);          // debug value

  /*
    servoMotor.write(45);
    delay(500);

    servoMotor.write(135);
    delay(500);


    for(int i = 0; i<180; i+=5){
      servoMotor.write(i);
      delay(150);
    }

    delay(1000);

    for(int i = 180; i>0; i-=5){
      servoMotor.write(i);
      delay(150);
    }
  */
}