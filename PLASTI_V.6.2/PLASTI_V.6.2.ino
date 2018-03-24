//NAME YOUR PLASTI
//////////////////////////////////////////////
int plastiNumber = 19;
//////////////////////////////////////////////
//////////////////////////////////////////////

//VARS
//////////////////////////////////////////////
struct dataStruct {
  int IO;
}
myData;
long restroll;
long restacc;
long roll;
long roll2;
long acc;
long acc2;
long absroll;
long absroll2;
long absacc;
long absacc2;
int movement;
int count;
byte addresses[][6] = {
  "1", "2", "3", "4", "5", "8",
  "9", "10", "19", "20", "21",
  "22", "24", "25", "26", "40",
  "41", "42", "43", "44", "45",
  "46", "47", "61", "62", "63", "66",
  "90", "91", "92", "93", "94", "95"
};

//////////////////////////////////////////////
//////////////////////////////////////////////

//STEPPER
//////////////////////////////////////////////
#include <AccelStepper.h>
#define HALFSTEP 8
#define motorPin1  3
#define motorPin2  4
#define motorPin3  5
#define motorPin4  6
AccelStepper stepper(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

//RADIO
//////////////////////////////////////////////
#include <SPI.h>
#include "RF24.h"
bool radioNumber = 1;
RF24 radio(7, 8);
bool role = 0;

//GYROSCOPE
//////////////////////////////////////////////
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int left, right;

void setup() {
//SERIAL
//////////////////////////////////////////////
  Serial.begin(9600);
  delay(100);
  Serial.println("hello");
  Serial.println("serial port online");
  myData.IO = 1;


  ////GYRO INITIALIZATION
  //////////////////////////////////////////////
  Wire.begin();
  mpu.initialize();
  while (mpu.testConnection() != 1) {
    mpu.initialize();
    delay(500);
  }
  mpu.setDMPEnabled(false);

  //RADIO INITIALIZATION
  //////////////////////////////////////////////
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108);
  radio.openReadingPipe(1, addresses[8]);
  radio.startListening();

  //UPSIDEDOWN HOLD
  //////////////////////////////////////////////
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  if (ax < 3000) {
    while (ax < 3000) {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    }
    stepper.enableOutputs ();
    delay(5000);
  }

  //RADIO READ
  //////////////////////////////////////////////
  if (radio.available()) {
    if (myData.IO == 0) {
      while (myData.IO == 0) {
        radio.read ( &myData, sizeof(myData) );
      }
    }
  }

  //WAIT FOR DRIFT
  //////////////////////////////////////////////
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  restacc = gz;
  delay(200);
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  while (abs(abs(restacc) - abs(gz)) > 10) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    restacc  = gz;
    delay(100);
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  }


  //FIND REST ACCEL AND ROLL
  //////////////////////////////////////////////
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  restroll = ay;
  delay(500);
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  while (abs(abs(restroll) - abs(ay)) > 10) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    restroll = ay;
    delay(500);
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  }
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  roll = -(ay - restroll);

  ///STEPPER INITIALIZATION
  //////////////////////////////////////////////
  stepper.setMaxSpeed(2000.0);
  stepper.setAcceleration(500.0);
  stepper.setCurrentPosition(0);
  delay(100);

  //GET THINGS GOING
  //////////////////////////////////////////////
  stepper.moveTo(50);
  while (stepper.distanceToGo() != 0) {
    stepper.setSpeed(400);
    stepper.runSpeedToPosition();
  }
  stepper.moveTo(-50);
  while (stepper.distanceToGo() != 0) {
    stepper.setSpeed(400);
    stepper.runSpeedToPosition();
  }
  stepper.moveTo(50);
  while (stepper.distanceToGo() != 0) {
    stepper.setSpeed(400);
    stepper.runSpeedToPosition();
  }
  stepper.moveTo(0);
  while (stepper.distanceToGo() != 0) {
    stepper.setSpeed(400);
    stepper.runSpeedToPosition();
  }
  Serial.println("START loop");
}


void loop() {

  //MOVEMENT LOOPS
  //////////////////////////////////////////////
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  roll = -(ay - restroll);
  absacc = abs(gz - restacc);
  absroll = abs(roll);
  acc = (gz - restacc);

  if (absroll < 150 && absacc < 300) {
    stepper.moveTo(roll);
    while (stepper.distanceToGo() != 0) {
      stepper.setSpeed(800);
      stepper.runSpeedToPosition();
    }
    delay(100);
    Serial.println("AAAHHHJH");
  }

  else if (absacc > 300 && (roll * acc) > 0 && absroll < 200) {
    delay(absroll / 100);
    Serial.print(roll);
    Serial.print("  ");
    Serial.println(acc);
    if (acc > 0) {
      movement = 150;
    }
    else {
      movement = -150;
    }


    movement += roll / 10;
    if (absroll > 800) {
      delay(10);
    }

    stepper.moveTo(movement);
    while (stepper.distanceToGo() != 0) {
      stepper.setSpeed(800);
      stepper.runSpeedToPosition();
    }
  }

  //UPSIDEDOWN HOLD
  //////////////////////////////////////////////
  if (ax < 5000) {
    Serial.print("UD");
    stepper.disableOutputs ();
    while (ax < 3000) {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    }
    stepper.enableOutputs ();
    delay(2000);
  }


  //RADIO INTERRUPT
  //////////////////////////////////////////////
  if (radio.available()) {
    radio.read ( &myData, sizeof(myData) );
    if (myData.IO == 0) {
      stepper.moveTo(0);
      while (stepper.distanceToGo() != 0) {
        stepper.setSpeed(800);
        stepper.runSpeedToPosition();
      }
      stepper.disableOutputs ();
      while (myData.IO == 0) {
        radio.read ( &myData, sizeof(myData) );
        stepper.enableOutputs ();
        delay(1000);
        stepper.disableOutputs ();
        delay(1000);
      }
      stepper.enableOutputs ();
      delay(500);
      ///////////////////////////////////////////////
      //WAIT FOR DRIFT
      //////////////////////////////////////////////
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      restacc = gz;
      delay(200);
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      while (abs(abs(restacc) - abs(gz)) > 10) {
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        restacc  = gz;
        delay(100);
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      }


      //FIND REST ACCEL AND ROLL
      //////////////////////////////////////////////
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      restroll = ay;
      delay(500);
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      while (abs(abs(restroll) - abs(ay)) > 10) {
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        restroll = ay;
        delay(500);
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      }
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      roll = -(ay - restroll);

      ///STEPPER INITIALIZATION
      //////////////////////////////////////////////
      stepper.setMaxSpeed(2000.0);
      stepper.setAcceleration(500.0);
      stepper.setCurrentPosition(0);
      delay(100);
    }

  }
}


















