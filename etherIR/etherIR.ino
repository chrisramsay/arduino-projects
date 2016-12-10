// etherIR.ino

#include <UIPEthernet.h>
#include <MLX90614.h>
#include <Wire.h>
#include <OneWire.h>
#include <dht.h>
//#include <DallasTemperature.h>

// Set up server on port 1000
EthernetServer server = EthernetServer(1000);

// DHT 22
#define DHT22_PIN 2
dht DHT;

// Other stuff
#define ONE_WIRE_BUS 3
#define DS18B20_ID 0x28
float t_ds;

  
OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
MLX90614 mlx;

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  
  // Begin the wire
  Wire.begin();
}
 
 
void loop(void)
{
  size_t size;
  float t_baa;
  float t_bcf;
  float dewpt;
//  float t_ds;

  // Get the temps
  //sensors.begin();
  //sensors.requestTemperatures();
  //t_ds = sensors.getTempCByIndex(0);
  t_ds = getTemperature();
  t_baa = mlx.getObjTemp(0x55);
  t_bcf = mlx.getObjTemp(0x5A);
  DHT.read22(DHT22_PIN);
  dewpt = dewPoint(DHT.temperature, DHT.humidity);


  if (EthernetClient client = server.available())
    {
      while((size = client.available()) > 0)
        {
          uint8_t* msg = (uint8_t*)malloc(size);
          size = client.read(msg,size);
          Serial.write(msg,size);
          free(msg);
        }
      client.print(t_baa);
      client.print(",");
      client.print(t_bcf);
      client.print(",");
      client.print(t_ds);
      client.print(",");
      client.stop();
    } 
}


boolean getTemperature(){
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  //find a device
  if (!oneWire.search(addr)) {
    oneWire.reset_search();
    return false;
  }
  if (OneWire::crc8( addr, 7) != addr[7]) {
    return false;
  }
  if (addr[0] != DS18B20_ID) {
    return false;
  }
  oneWire.reset();
  oneWire.select(addr);
  // Start conversion
  oneWire.write(0x44, 1);
  // Wait some time...
  delay(850);
  present = oneWire.reset();
  oneWire.select(addr);
  // Issue Read scratchpad command
  oneWire.write(0xBE);
  // Receive 9 bytes
  for ( i = 0; i < 9; i++) {
    data[i] = oneWire.read();
  }
  // Calculate temperature value
  t_ds = ( (data[1] << 8) + data[0] )*0.0625;
  return true;
}

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
    RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
    RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
    RHS += log10(1013.246);

    // factor -3 is to adjust units - Vapor Pressure SVP * humidity
    double VP = pow(10, RHS - 3) * humidity;

    // (2) DEWPOINT = F(Vapor Pressure)
    double T = log(VP/0.61078);   // temp var
    return (241.88 * T) / (17.558 - T);
}