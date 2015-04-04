#include <OneWire.h>
#include <DallasTemperature.h>
#include <UIPEthernet.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 3
// Assign Arduino MAC address
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// IP address for Arduino
byte ip[] = {192, 168, 100, 6 };
// Internet access via router
byte gateway[] = {192, 168, 100, 1 }; 
// Subnet mask
byte subnet[] = {255, 255, 255, 0 };
// Server port arduino server will use
EthernetServer server(1000);
// Ethernet client object
EthernetClient client;
// Checkpoint is IP requested for by check alive 
byte checkpoint[] = { 192, 168, 100, 2 };
// Pin to be pulsed for 555 keepalive
int pulsePin = 2;
// Visible heartbeat
int heartbeat_led = 4;
// Reset time in millis
int reset_millis = 20000;
// Last heartbeat time
unsigned long lastHeartbeat = 0;
// Last uptime report
unsigned long lastUptimeReport = 0;
// OneWire bus
OneWire  oneWire(ONE_WIRE_BUS);

// Probes
// 15 cm
byte probe_15[] = { 0x28, 0xC8, 0xCC, 0xBF, 0x04, 0x00, 0x00, 0xB6 };
// 30 cm
byte probe_30[] = { 0x28, 0xBA, 0x17, 0xD0, 0x04, 0x00, 0x00, 0x64 };
// 100 cm
byte probe_100[] = { 0x28, 0xE3, 0x67, 0xCE, 0x04, 0x00, 0x00, 0x9C };
// Grass
byte probe_grass[] = { 0x28, 0xF5, 0xE4, 0x83, 0x05, 0x00, 0x00, 0x26 };
// Concrete
byte probe_concrete[] = { 0x28, 0xE7, 0x17, 0x11, 0x05, 0x00, 0x00, 0x1D };
// Internal
byte probe_internal[] = { 0x28, 0xDB, 0xD4, 0x8C, 0x05, 0x00, 0x00, 0x9D };

// Indicator LEDs
int led_15 = 9;
int led_30 = 8;
int led_100 = 7;
int led_grass = 6;
int led_concrete = 5;

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Uncomment for verbose output
 #define DEBUG
#ifdef DEBUG
  #define DEBUG_PRINT(x)  Serial.println (x)
#else
  #define DEBUG_PRINT(x)
#endif
//////////////////////

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  // Debug message
  DEBUG_PRINT("/\\\\\\\\\\\\\\\\/ RESET /\\\\\\\\\\\\\\\\/");
  // Indicators
  pinMode(led_15, OUTPUT);
  pinMode(led_30, OUTPUT);
  pinMode(led_100, OUTPUT);
  pinMode(led_grass, OUTPUT);
  pinMode(led_concrete, OUTPUT);
  // Assign hearbeat output pin as output
  pinMode(heartbeat_led, OUTPUT);
  // Start server
  Ethernet.begin(mac,ip,gateway,gateway,subnet); 
  server.begin();
  // Start up the library
  sensors.begin();
  // set the resolution to 12 bit
  sensors.setResolution(probe_15, 12);
  sensors.setResolution(probe_30, 12);
  sensors.setResolution(probe_100, 12);
  sensors.setResolution(probe_grass, 12);
  sensors.setResolution(probe_concrete, 12);
  sensors.setResolution(probe_internal, 12);
  // Initial hearbeat
  heartbeat("Initial heartbeat");

}

//////////////////////

void loop(void)
{
  size_t size;
  float t_15;
  float t_30;
  float t_100;
  float t_grass;
  float t_concrete;
  float t_internal;

  // Get temperature outside the ethernet part
  sensors.requestTemperatures();
  t_15 = sensors.getTempC(probe_15);
  t_30 = sensors.getTempC(probe_30);
  t_100 = sensors.getTempC(probe_100);
  t_grass = sensors.getTempC(probe_grass);
  t_concrete = sensors.getTempC(probe_concrete);
  t_internal = sensors.getTempC(probe_internal);

  // Check lights - this maps a device to a LED
  device_check(t_15, led_15);
  device_check(t_30, led_30);
  device_check(t_100, led_100);
  device_check(t_grass, led_grass);
  device_check(t_concrete, led_concrete);

  // THIS PART RUNS EVERY reset_millis/1000 SECONDS
  unsigned long uptime = millis();
  if ((uptime - lastUptimeReport) >= reset_millis) {
    checkAlive();
    lastUptimeReport = (uptime - (uptime % reset_millis));
  }

  // This will output data on request
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //if HTTP request has ended
        if (c == '\n') {
          // Output sensor data
          client.print(t_15);
          client.print(", ");
          client.print(t_30);
          client.print(", ");
          client.print(t_100);
          client.print(", ");
          client.print(t_grass);
          client.print(", ");
          client.print(t_concrete);
          client.print(", ");
          client.print(t_internal);
          client.stop();
          // Small delay
          delay(1);
          //stopping client
          client.stop();

        }
      }
    }
  }

  // Small delay in between loops
  delay(500);
}

//////////////////////////

void checkAlive() //client function to send and receive GET data from external server.
{
  if (client.connect(checkpoint, 22)) {
    heartbeat("28J60 is alive");
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

void device_check(float t, int led)
{
  if (t == 85.0 || t == -127.0) {
    alert(led);
  } else {
    ok(led);
  }
}

void heartbeat(String message)
{
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

void alert(int pin)
{
  // Makes RED
  digitalWrite(pin, HIGH);
}

void ok(int pin)
{
  // Makes GREEN
  digitalWrite(pin, LOW);
}
