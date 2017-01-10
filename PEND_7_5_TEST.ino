int plastiNumber=1;

struct dataStruct{
  int IO;
}
myData;

long restroll;
long roll;
int movement;
long restacc;
long acc;

#include <AccelStepper.h>    
#define HALFSTEP 8  
#define motorPin1  3     
#define motorPin2  4
#define motorPin3  5
#define motorPin4  6   
AccelStepper stepper(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);  

#include <SPI.h>
#include "RF24.h"
bool radioNumber = 1;
RF24 radio(7,8);
bool role = 0;


#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz; 
int left, right;

void setup() {

  myData.IO = 1;

  Serial.begin(9600);
  delay(100);
  Serial.println("hello");
  Serial.println("serial port online");

  ////GYRO INITIALIZATION///////////////////////////////////////////////
  Wire.begin(); 
  Serial.println("wire began");

  mpu.initialize();
  Serial.println("testing mpu");

  while(mpu.testConnection() != 1){ 
    mpu.initialize();
    Serial.println("...again");

  }
  mpu.setDMPEnabled(false);
  delay(500);
  
    //RADIO INITIALIZATION///////////////////////////////////////////////
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);  
  radio.setChannel(108);
  radio.openReadingPipe(1, plastiNumber);
  radio.startListening();
  Serial.println("radio online");
  ///////////////////////////////////////////////

  ///STEPPER INITIALIZATION/////////////////////////////////////////////// 
  stepper.setMaxSpeed(800.0);  
  stepper.setAcceleration(2000.0); 
  stepper.setCurrentPosition(0);
  delay(100);
  Serial.println("stepper initiallized");
  ///////////////////////////////////////////////
  
  //UPSIDEDOWN HOLD//////////////////////////////////////////
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  if(ax<3000){
    Serial.println("upsidedown");
    while(ax<3000){
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    }
    stepper.enableOutputs ();
    delay(5000);
  }

  //RADIO READ/////////////////////////////////////////////
  if (radio.available()){
    if(myData.IO == 0){
      while(myData.IO == 0){
        radio.read ( &myData, sizeof(myData) ); 
      }
    }
  }
  
  ///////////////////////////////////////////////
  Serial.print("waiting for drift to calm");
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  restacc = gz;
  delay(200);
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  while(gz != restacc){
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    restacc  = gz;
    delay(100);
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    Serial.print("."); 
  }

  Serial.println("gyro initiallized");
  /////////////////////////////////////////////// 

  ////FIND REST ACCEL AND ROLL///////////////////////////////////////////////
  Serial.print("finding rest roll."); 
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  restroll = ay;
  delay(500);
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  while(abs(abs(restroll)-abs(ay))>20){
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    restroll = ay;
    delay(500);
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    Serial.print(".");
  }

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  roll = -(ay - restroll);

  Serial.print("rest roll = ");  
  Serial.println(restroll);
  delay(100);
  Serial.print("roll = ");  
  Serial.println(roll);
  delay(500);
  ///////////////////////////////////////////////



  Serial.println("MOVING!");

  stepper.moveTo(200);
  while(stepper.distanceToGo() != 0){ 
    stepper.setSpeed(600);
    stepper.runSpeedToPosition();
  }
  stepper.moveTo(-200);
  while(stepper.distanceToGo() != 0){ 
    stepper.setSpeed(600);
    stepper.runSpeedToPosition();
  }
  stepper.moveTo(0);
  while(stepper.distanceToGo() != 0){ 
    stepper.setSpeed(800);
    stepper.runSpeedToPosition();
  }
  Serial.println("automation initiated");
}
//////////////////////////////////////////////////////////////////////////////////////////  
//////////////////////////////////////////////////////////////////////////////////////////  


void loop() {
  ////MOVEMENT LOOPS//////////////////////////////////////////////////////////////////////////////////////////

  if(abs(stepper.currentPosition())>350){
    stepper.moveTo(0);
    while(stepper.distanceToGo()!=0){
      stepper.setSpeed(1000);
      stepper.runSpeedToPosition(); 
    }
  }

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  roll = -(ay - restroll);
  acc = abs(gz - restacc);


  if(abs(roll)<300 && acc<200){
  stepper.moveTo(roll/2);
  while(stepper.distanceToGo()!=0){
  stepper.setSpeed(1000);
  stepper.runSpeedToPosition();
  }
  }
  else{
   if(roll<=0){
    movement = -160;
  }
  
   if(roll>0){
    movement = 160;
  }
  if(acc>100){
  stepper.moveTo(movement);
  stepper.setSpeed(acc);
  stepper.runSpeedToPosition();
  }
  }
  ////////////////////////////////////////////////////////////

  //UPSIDEDOWN HOLD//////////////////////////////////////////
  if(ax<5000){
    stepper.disableOutputs ();
    while(ax<3000){
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    }
    stepper.enableOutputs ();
    delay(2000);
  }
  ///////////////////////////////////////////////


  ///RADIO INTERRUPT//////////////////////////////////////////// 
  if (radio.available()){
    radio.read ( &myData, sizeof(myData) ); 

    if(myData.IO == 0){
      stepper.disableOutputs ();
      while(myData.IO == 0){
        if (radio.available()){
          radio.read ( &myData, sizeof(myData) ); 
        }  
      }
      stepper.enableOutputs ();
      delay(500);
      stepper.moveTo(100);
      while(stepper.distanceToGo() != 0){ 
        stepper.setSpeed(100);
        stepper.runSpeedToPosition();
      }
      stepper.moveTo(0);
      while(stepper.distanceToGo() != 0){ 
        stepper.setSpeed(800);
        stepper.runSpeedToPosition();
      }
    } 
  }

}















