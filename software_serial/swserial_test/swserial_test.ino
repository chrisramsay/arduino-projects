#include <SoftwareSerial.h>
int setL, setR;

SoftwareSerial mySerial(4, 5); // RX, TX

void setup()  
{
  
  Serial.begin(9600);
  Serial.flush();
  mySerial.begin(9600);
  mySerial.flush();
  
}

void loop() 
{
  if (mySerial.available()) {   //read the two speeds

    int setL = mySerial.parseInt(); 
    int setR = mySerial.parseInt(); 
    
    if (mySerial.read() == '\n') {
      Serial.print("setL = ");
      Serial.print(setL);
      Serial.print(", setR = ");
      Serial.println(setR);
    }  
 }
  delay(100);
}
