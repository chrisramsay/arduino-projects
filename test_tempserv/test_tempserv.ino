/*
  DS18b20 temperature webserver.
 The webserve part of this code is taked from the example provided in the Dallas library examples
 
 How it works.
 After initial setup, the looping code fetches the tempeartures form the sensors on each pass.
 When the code detect an ethernet client a page id returned containing whatever device array has stored.
 This way we never fetch the live values during a request.
 
 John Burrin March 2015
 */

#include <SPI.h>
#include <UIPEthernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 3
#define TEMPERATURE_PRECISION 12
#define MAX_DEVICES 10

// comment out the next line to disable serial debug output
#define DEBUG
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

//  a structure to hold device specific information
int sensorCount = 0;
struct device{
  DeviceAddress address;  // The address of the device 
  float tempC;            // A place to store it's last temp reading
};

// Maximum number of devices we will have on the sensor array
// This is just because C needs an array size when declared and I'm not using pointers
device devices[MAX_DEVICES];

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = {192, 168, 100, 7 };

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(1000);


// untility function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}


void setup() {
  // Open serial communications and wait for port to open:
  // start serial port
  Serial.begin(9600);

  // start the Ethernet connection and the server:

#ifdef DEBUG
  Serial.println("Starting up Webserver");
#endif

  Ethernet.begin(mac, ip);
  server.begin();

#ifdef DEBUG
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  Serial.println("Done");
  Serial.println("Initialise Sensors");
#endif
  // locate devices on the bus
#ifdef DEBUG
  Serial.print("Locating devices...");
#endif

  sensors.begin();
  sensorCount = sensors.getDeviceCount();
#ifdef DEBUG
  Serial.print("Found "); 
  Serial.print(sensorCount, DEC);
  Serial.println(" devices.");


  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
#endif

#ifdef DEBUG
  Serial.println("Setting Sensor precision...");
#endif

  // Set precsion of all sensors
  sensors.setResolution(TEMPERATURE_PRECISION);
  // Interrate over the sensors by index, get the address, set the precsion
  for (int loop=0; loop < sensorCount; loop++){
#ifdef DEBUG
    sensors.getAddress(devices[loop].address, loop);
#endif
    //  sensors.setResolution(devices[loop].address, TEMPERATURE_PRECISION);
#ifdef DEBUG
    Serial.print("Device Address: ");
    printAddress(devices[loop].address);
    Serial.println(" ");
#endif
  }
}


void loop() {
#ifdef DEBUG
  Serial.println("Requesting temperatures...");
#endif  
  sensors.requestTemperatures(); // Send the command to get temperatures

    // Get the temperature in C of each sensor
  for(int loop = 0; loop < sensorCount; loop++){
    devices[loop].tempC = sensors.getTempCByIndex(loop);
    // devices[loop].tempC = sensors.getTempC(devices[loop].address);
  }
  // output the value of each device
#ifdef DEBUG
  Serial.print(devices[0].tempC);
  for (int loop = 1; loop < sensorCount; loop++){
    Serial.print(",");
    Serial.print(devices[loop].tempC);
  }
  Serial.println(" ");
#endif
  // listen for incoming clients
  EthernetClient client = server.available();

  if (client) {
#ifdef DEBUG
    Serial.println("new client");
#endif
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
#ifdef DEBUG
        Serial.write(c);
#endif
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();
          // output the value of each device
#ifdef DEBUG
          Serial.print(devices[0].tempC);
#endif
          client.print(devices[0].tempC);
          for (int loop = 1; loop < sensorCount; loop++){
#ifdef DEBUG
            Serial.print(",");
            Serial.print(devices[loop].tempC);
#endif
            client.print(",");
            client.print(devices[loop].tempC);
          }

          //client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
#ifdef DEBUG
    Serial.println(" ");
    Serial.println("client disconnected");
#endif
  }
}
