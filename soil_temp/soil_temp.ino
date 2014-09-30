#include <OneWire.h>
#include <DallasTemperature.h>
#include <UIPEthernet.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 3
// Uncomment for verbose output
// #define DEBUG

OneWire  oneWire(ONE_WIRE_BUS);

// Probes
DeviceAddress add1 = { 0x28, 0xD9, 0x19, 0x11, 0x05, 0x00, 0x00, 0x41 };
DeviceAddress add2 = { 0x28, 0xF5, 0xE4, 0x83, 0x05, 0x00, 0x00, 0x26 };
DeviceAddress add3 = { 0x28, 0xE7, 0x17, 0x11, 0x05, 0x00, 0x00, 0x1D };

// Indicator LEDs
int led_15 = 9;
int led_30 = 8;
int led_100 = 7;

// Set up server on port 1000
EthernetServer server = EthernetServer(1000);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void setup(void)
{
  // start serial port
  Serial.begin(9600);

  // Indicators
  pinMode(led_15, OUTPUT);
  pinMode(led_30, OUTPUT); 
  pinMode(led_100, OUTPUT);

  // Server initialisation
  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  IPAddress myIP(192,168,100,6);
  Ethernet.begin(mac,myIP);

  // Start Server
  server.begin();

  // Start up the library
  sensors.begin();
  // set the resolution to 10 bit (good enough?)
  sensors.setResolution(add1, 12);
  sensors.setResolution(add2, 12);
  sensors.setResolution(add3, 12);

}
 
 
void loop(void)
{
  size_t size;
  float t_15;
  float t_30;
  float t_100;

  // Get temperature outside the ethernet part
  sensors.requestTemperatures();
  t_15 = sensors.getTempC(add1);
  t_30 = sensors.getTempC(add2);
  t_100 = sensors.getTempC(add3);

  // Check lights
  device_check(t_15, led_15);
  device_check(t_30, led_30);
  device_check(t_100, led_100);

  if (EthernetClient client = server.available()) {
    while((size = client.available()) > 0) {
      uint8_t* msg = (uint8_t*)malloc(size);
      size = client.read(msg,size);
      Serial.write(msg,size);
      free(msg);
    }
    client.print(t_15);
    client.print(", ");
    client.print(t_30);
    client.print(", ");
    client.print(t_100);
    client.print(", ");
    client.stop();
  }

    Serial.print(t_15);
    Serial.print(", ");
    Serial.print(t_30);
    Serial.print(", ");
    Serial.print(t_100);
    Serial.println(", ");

}

void device_check(float t, int led) {
#ifdef DEBUG
  delay(1000);
#endif
  if (t == 85.0 || t == -127.0) {
    alert(led);
  } else {
    ok(led);
  }
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
