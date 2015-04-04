#include <UIPEthernet.h>

// Assign Arduino MAC address
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// IP address for Arduino
byte ip[] = {192, 168, 100, 9 };
// Internet access via router
byte gateway[] = {192, 168, 100, 1 }; 
// Subnet mask
byte subnet[] = {255, 255, 255, 0 };
// Server port arduino server will use
EthernetServer server(84);
// Ethernet client object
EthernetClient client;
// Checkpoint is IP requested for by check alive 
byte checkpoint[] = { 192, 168, 100, 2 };
// Pin to be pulsed for 555 keepalive
int pulsePin = 2;
// Visible heartbeat
int heartbeat_led = 4;
// Reset time in millis
int reset_millis = 60000;
// Last heartbeat time
unsigned long lastHeartbeat = 0;
// Last uptime report
unsigned long lastUptimeReport = 0;
// Uncomment for verbose output
 #define DEBUG
#ifdef DEBUG
  #define DEBUG_PRINT(x)  Serial.println (x)
#else
  #define DEBUG_PRINT(x)
#endif
//////////////////////

void setup(){
  // Assign hearbeat output pin as output
  pinMode(heartbeat_led, OUTPUT);
  // Start server
  Ethernet.begin(mac,ip,gateway,gateway,subnet); 
  server.begin();
  // Start serial output
  Serial.begin(9600);
  DEBUG_PRINT("/\\\\\\\\\\\\\\\\/ RESET /\\\\\\\\\\\\\\\\/");
  heartbeat("Initial heartbeat");
}

void loop(){

  // THIS PART RUNS EVERY reset_millis/1000 SECONDS
  unsigned long uptime = millis();
  if ((uptime - lastUptimeReport) >= reset_millis) {
  	checkAlive();
    lastUptimeReport = (uptime - (uptime % reset_millis));
  }

  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //if HTTP request has ended
        if (c == '\n') {

          //now output HTML data header   
          client.println("Hello, World!");

          delay(1);
          //stopping client
          client.stop();

        }
      }
    }
  }
  // delay in between loops
  delay(500);
} 

//////////////////////////

void checkAlive() //client function to send and receive GET data from external server.
{
  if (client.connect(checkpoint, 80)) {
  	heartbeat("28J60 alive");
    client.println("GET / HTTP/1.1");
    client.println();
  } 
  else {
    DEBUG_PRINT("connection failed");
    DEBUG_PRINT();
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read();
    Serial.print(c);
  }

  DEBUG_PRINT("disconnecting.");
  client.stop();
}

void heartbeat(String message) {
  // Sink current to drain charge from watchdog circuit
  pinMode(pulsePin, OUTPUT);
  digitalWrite(pulsePin, LOW);
  digitalWrite(heartbeat_led, HIGH);
  delay(300);
  // Return to high-Z
  pinMode(pulsePin, INPUT);
  digitalWrite(heartbeat_led, LOW);
  lastHeartbeat = millis();
  DEBUG_PRINT(message);
}