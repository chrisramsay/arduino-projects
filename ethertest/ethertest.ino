#include <SPI.h>
#include <Ethernet.h>


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,100,21);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // You can't use pin 13 and the w5100 uses it so initialize digital pin 2 as an output instead.
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  Serial.println("Starting up Webserver");
   Ethernet.begin(mac, ip);
   server.begin();
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(2, HIGH);
  Serial.println("BLINK");
  delay(1000);          
  digitalWrite(2, LOW);  
  delay(1000);         
}
