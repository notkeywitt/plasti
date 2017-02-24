#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
  MPU6050 mpu;
  int16_t ax, ay, az;
  int16_t gx, gy, gz; 
  
#include <SPI.h>
#include "RF24.h"
bool radioNumber = 1;
RF24 radio(7,8);
bool role = 0;

int w;
long roll;
long restroll;
long restacc;
long acc;
long vel;
long motion;
long greatest;

void setup() {
Serial.begin(9600);
delay(500);
Wire.begin(); 
mpu.initialize();
mpu.setDMPEnabled(false);
delay(5000);
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  restacc = gz;
  delay(400);
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  while(gz != restacc){
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    restacc  = gz;
    delay(400);
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    Serial.print("."); 
  }
mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
restroll = ay;
delay(500);
mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
while(abs(abs(restroll)-abs(ay))>20){
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  restroll = ay;

  delay(500);
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print(F("Roll Spread :"));Serial.println(abs(abs(restroll)-abs(ay)));
  }
delay(1000);

}

void loop() {

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  roll = -(ay - restroll);
  acc = (gz - restacc);

if(roll>greatest){
  greatest =roll;
}
  Serial.print(" greatest: ");
Serial.print(greatest);
Serial.print("   ");
Serial.print(acc);
Serial.print("  ");
Serial.print("   ");
Serial.println(roll);
delay(10);

}
