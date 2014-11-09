#include <UIPEthernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 100, 8 };
byte endpoint[] = { 192, 168, 100, 2 };

int pulsePin = 2;
int led = 4;
int reset_millis = 10000;
unsigned long lastHeartbeat = 0;
unsigned long lastUptimeReport = 0;

EthernetClient client;
EthernetServer server = EthernetServer(1000);

void setup()
{
  Ethernet.begin(mac, ip);

  // start listening for clients
  server.begin();

  Serial.begin(9600);

  delay(1000);

  Serial.println("/////////////////// RESET ///////////////////");
  heartbeat("Initial heartbeat");

}

void loop()
{

  size_t size;
  // if an incoming client connects, there will be bytes available to read:
  EthernetClient incoming = server.available();
  if (incoming == true) {
    Serial.println("incoming is TRUE");
    while((size = incoming.available()) > 0) {
      uint8_t* msg = (uint8_t*)malloc(size);
      size = incoming.read(msg,size);
      Serial.write(msg,size);
      free(msg);
    }
    incoming.print("Hello, World!");
    incoming.stop();
  } else {
    Serial.print("incoming FALSE");
  }

  // THIS PART RUNS EVERY reset_millis/1000 SECONDS
  unsigned long uptime = millis();
  if ((uptime - lastUptimeReport) >= reset_millis) {
    Serial.println("connecting...");
    if (client.connect(endpoint, 80)) {
      heartbeat("connected");
    } else {
      Serial.println("connection failed");
    }

    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();
      for(;;)
        ;
    }
    lastUptimeReport = (uptime - (uptime % reset_millis));
  }
  // delay in between loops
  delay(500);
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