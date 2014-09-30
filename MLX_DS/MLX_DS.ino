#include <MLX90614.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 3
  
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
MLX90614 mlx;

void setup() {
    Serial.begin(9600);
    Wire.begin();
    Serial.println("ID, to, ta, ID, to, ta, tds");
}

void loop() {
    // DS18B20
    sensors.begin();
    sensors.requestTemperatures(); 
    // Set MLX addresses (0x55 is the factory set default address)
    measure(0x55);
    measure(0x5A); 
    // Get DS18B20 
    Serial.print(sensors.getTempCByIndex(0));
    Serial.println("");
    // Wait a little
    delay(1000);

}

void measure(int address) {
	// Show which one we are measuring
	Serial.print(address);
    Serial.print(",");
    // Get object temperature
    Serial.print(mlx.getObjTemp(address));
    Serial.print(",");
    // Get ambient temperature
    Serial.print(mlx.getAmbTemp(address));
    Serial.print(",");	
}