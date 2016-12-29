#include <dht.h>
#include <OneWire.h>

dht DHT;
int DS18S20_Pin = 8;

//Temperature chip i/o
OneWire ds(DS18S20_Pin);

#define DHT22_PIN 6
#define OFF_PIN 2
#define ON_PIN 3
#define HYSTERESIS 2.0

void setup()
{
  pinMode(OFF_PIN, OUTPUT);
  pinMode(ON_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("DHT TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
  Serial.println("stat, Hum (%), Temp (C), Dew (C), Surf (C), Heat");
}

void loop()
{
  // READ DATA

  float temperature = getTemp();
  int chk = DHT.read22(DHT22_PIN);
  float dew = dewPoint(DHT.temperature, DHT.humidity);
  int heaterState = LOW;
  int goFlag = LOW;

  switch (chk)
  {
    case DHTLIB_OK:
      Serial.print("OK,\t");
      // Now check DS18B20
      if (temperature < 85.0 && temperature > -127.0) {
        goFlag = HIGH;
      }
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      break;
    default:
      Serial.print("Unknown error,\t");
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
    // DISPLAY DATA
    Serial.print(DHT.humidity, 1);
    Serial.print(",\t");
    Serial.print(DHT.temperature, 1);
    Serial.print(",\t");
    Serial.print(dew, 1);
    Serial.print(",\t");
    Serial.print(temperature);
    Serial.print(",\t");
    Serial.println(heaterState);
  } else {
    heaterState = LOW;
    digitalWrite(OFF_PIN, 1);
    digitalWrite(ON_PIN, 0);
    Serial.println("Safety cut off");
  }
  delay(2500);
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
    return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized");
    return -1000;
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
