
#include <SPI.h>
#include "RF24.h"


RF24 radio (7, 8); 
byte addresses[][6] = {
"1","2","3","4", "5", "8", 
"9", "10", "19","20","21",
"22","24", "25", "26","40",
"41","42","43","44","45",
"46","47","61","62","63","66",
"90","91","92","93","94","95"}; 

int pause = 1000;
int count; 
int IO = 1;
int switchPin = 2;
int pipe;

void setup() {
  Serial.begin(9600);
  pinMode(switchPin, INPUT_PULLUP);
  radio.begin();  
  radio.setChannel(108); 
 // radio.setPALevel(RF24_PA_MIN);0
  radio.setPALevel(RF24_PA_MAX);  
  radio.openWritingPipe(19);
  }

void loop() {
  count = 0;
  pipe = 0;
  if(digitalRead(switchPin) == LOW){
    Serial.println("OFF"); 
    IO = 0;
    while(pipe<34){  
         Serial.println(pipe); 
         Serial.println((char*)addresses[pipe]); 
      radio.openWritingPipe(addresses[pipe]);
      radio.write( &IO, sizeof(IO) );
      radio.openWritingPipe(addresses[pipe]);
      radio.write( &IO, sizeof(IO) );
      count += 1;
      pipe += 1;
      
  }
  }
  
  
  if(digitalRead(switchPin) == HIGH){
    Serial.println("ON"); 
    IO = 1;
    
    while(pipe<20){     
           Serial.println((char*)addresses[pipe]);
             Serial.println(pipe);
      radio.openWritingPipe(addresses[pipe]);
      radio.write( &IO, sizeof(IO) );
      radio.openWritingPipe(addresses[pipe]);
      radio.write( &IO, sizeof(IO) );
      pipe += 1;
    }
  }

}



