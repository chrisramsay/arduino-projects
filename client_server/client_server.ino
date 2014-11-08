#include <UIPEthernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //assign arduino mac address
byte ip[] = {192, 168, 100, 9 }; // ip in lan assigned to arduino
byte gateway[] = {192, 168, 100, 1 }; // internet access via router
byte subnet[] = {255, 255, 255, 0 }; //subnet mask
EthernetServer server(84); //server port arduino server will use
EthernetClient client;
byte checkpoint[] = { 192, 168, 100, 2 }; // (IP) zoomkat web page server IP address

int pulsePin = 2;
int led = 4;
int reset_millis = 10000;
unsigned long lastHeartbeat = 0;
unsigned long lastUptimeReport = 0;

String readString; //used by server to capture GET request 

//////////////////////

void setup(){

  pinMode(led, OUTPUT);
  Ethernet.begin(mac,ip,gateway,gateway,subnet); 
  server.begin();
  Serial.begin(9600); 
  heartbeat("Initial heartbeat");
}

void loop(){

  // THIS PART RUNS EVERY reset_millis/1000 SECONDS
  unsigned long uptime = millis();
  if ((uptime - lastUptimeReport) >= reset_millis) {
  	checkAlive();
    lastUptimeReport = (uptime - (uptime % reset_millis));
  }
  // delay in between loops
  delay(500);
  // check for serial input
  if (Serial.available() > 0) 
  {
    byte inChar;
    inChar = Serial.read();
    if(inChar == 'g')
    {
      checkAlive(); // call client checkAlive function
    }
  }  

  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {

          //store characters to string 
          readString += c; 
          //Serial.print(c);
        } 

        //if HTTP request has ended
        if (c == '\n') {

          ///////////////
          Serial.print(readString); //print to serial monitor for debuging 

            //now output HTML data header
          if(readString.indexOf('?') >=0) { //don't send new page
            client.println(F("HTTP/1.1 204"));
            client.println();
            client.println();  
          }
          else {   
            client.println("Hello, World!");
          }

          delay(1);
          //stopping client
          client.stop();

        }
      }
    }
  }
} 

//////////////////////////
void checkAlive() //client function to send and receive GET data from external server.
{
  if (client.connect(checkpoint, 80)) {
  	heartbeat("connected");
    client.println("GET / HTTP/1.1");
    client.println();
  } 
  else {
    Serial.println("connection failed");
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read();
    Serial.print(c);
  }

  Serial.println("disconnecting.");
  client.stop();
}

void heartbeat(String message) {
  // Sink current to drain charge from watchdog circuit
  pinMode(pulsePin, OUTPUT);
  digitalWrite(pulsePin, LOW);
  digitalWrite(led, HIGH);
  delay(300);
  // Return to high-Z
  pinMode(pulsePin, INPUT);
  digitalWrite(led, LOW);
  lastHeartbeat = millis();
  Serial.println(message);
}