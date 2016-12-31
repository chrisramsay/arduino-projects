// SDA (BLUE) goes to DIGITAL PIN 2
// SCL (GREEN) goes to DIGITAL PIN 3

#include <i2cmaster.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
//t
#include <dht.h>
#include <OneWire.h>
//t

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

//t
dht DHT;
int DS18S20_Pin = 10;
int heaterState = LOW;

//Temperature chip i/o
OneWire ds(DS18S20_Pin);

#define DHT22_PIN 6
#define OFF_PIN 11
#define ON_PIN 12
#define HYSTERESIS 2.0
//t

void setup() {
  //t
  pinMode(OFF_PIN, OUTPUT);
  pinMode(ON_PIN, OUTPUT);
  //t
  Serial.begin(9600);
  i2c_init(); //Initialise the i2c bus
  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
}

void loop() {
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

//t
  float temperature = getTemp();
  int chk = DHT.read22(DHT22_PIN);
  float dew = dewPoint(DHT.temperature, DHT.humidity);
  int goFlag = LOW;

  switch (chk)
  {
    case DHTLIB_OK:
      // Now check DS18B20
      if (temperature < 85.0 && temperature > -127.0) {
        goFlag = HIGH;
      }
      break;
    case DHTLIB_ERROR_CHECKSUM:
      break;
    case DHTLIB_ERROR_TIMEOUT:
      break;
    default:
      break;
  }
  if (goFlag) {
    // Control heater state - is dew point higher than
    // surface temp minus hysteresis?
    // e.g. dew 16, surface '15' (actual 17 - 2 hyst.) -> ON
    if (dew > temperature - HYSTERESIS) {
      heaterState = HIGH;
      digitalWrite(OFF_PIN, 0);
      digitalWrite(ON_PIN, 1);
    } else {
      heaterState = LOW;
      digitalWrite(OFF_PIN, 1);
      digitalWrite(ON_PIN, 0);
    }
  } else {
    heaterState = LOW;
    digitalWrite(OFF_PIN, 1);
    digitalWrite(ON_PIN, 0);
  }
//t

  delay(1000); // Poll every 50ms
}

void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');

  // is "temp" command?
  // Example: http://arduino.local/arduino/temp/
  if (command == "temp") {
    tempCommand(client);
  }
}

void tempCommand(YunClient client) {
  // Read the command
  String mode = client.readStringUntil('\r');
  float surface = getTemp();
  float dewpoint = dewPoint(DHT.temperature, DHT.humidity);
  // Switch on mode...
  // Example: http://arduino.local/arduino/temp/all
  if (mode == "all") {
    client.print(F("Ambient:\t"));
    client.print(readtemp(0x06));
    client.println();
    client.print(F("Object:\t\t"));
    client.print(readtemp(0x07));
    client.println();
    client.print(F("Surface:\t"));
    client.print(surface);
    client.println();
    client.print(F("DHT temp:\t"));
    client.print(DHT.temperature);
    client.println();
    client.print(F("DHT hum:\t"));
    client.print(DHT.humidity);
    client.println();
    client.print(F("Dew point:\t"));
    client.print(dewpoint);
    client.println();
    client.print(F("Act diff:\t"));
    client.print(surface - dewpoint);
    client.println();
    client.print(F("Hyst diff:\t"));
    client.print(surface - dewpoint - HYSTERESIS);
    client.println();
    client.print(F("Heater:\t\t"));
    client.print(heaterState);
    client.println(); 
  }
  // Example: http://arduino.local/arduino/temp/csv
  if (mode == "csv") {
    client.print(readtemp(0x06));
    client.print(",");
    client.print(readtemp(0x07));
    client.print(",");
    client.print(surface);
    client.print(",");
    client.print(DHT.temperature);
    client.print(",");
    client.print(DHT.humidity);
    client.print(",");
    client.print(dewpoint);
    client.print(",");
    client.print(surface - dewpoint);
    client.print(",");
    client.print(surface - dewpoint - HYSTERESIS);
    client.print(",");
    client.print(heaterState);
    client.print(",");
    client.println(); 
  }
}

float readtemp(int sensor) {

  int dev = 0x5A << 1;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  i2c_start_wait(dev + I2C_WRITE);

  i2c_write(sensor);

  // read
  i2c_rep_start(dev + I2C_READ);
  data_low = i2c_readAck(); //Read 1 byte and then send ack
  data_high = i2c_readAck(); //Read 1 byte and then send ack
  pec = i2c_readNak();
  i2c_stop();

  // This converts high and low bytes together and processes temperature,
  // MSB is a error bit and is ignored for temps
  // 0.02 degrees per LSB (measurement resolution of the MLX90614)
  double tempFactor = 0.02;
  // zero out the data
  double tempData = 0x0000;
  // data past the decimal point
  int frac;

  // This masks off the error bit of the high byte,
  // then moves it left 8 bits and adds the low byte.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  return ((tempData * tempFactor) - 0.01) - 273.15;
}

//t
// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
double dewPoint(double celsius, double humidity)
{
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.90298 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);

  // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, RHS - 3) * humidity;

  // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP / 0.61078); // temp var
  return (241.88 * T) / (17.558 - T);
}

float getTemp()
{
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
    //no more sensors on chain, reset search
    ds.reset_search();
    return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return -1001;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized");
    return -1002;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad


  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

}
//t
