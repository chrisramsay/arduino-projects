// etherIR.ino

#include <UIPEthernet.h>
#include <MLX90614.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Set up server on port 1000
EthernetServer server = EthernetServer(1000);

// Other stuff
#define ONE_WIRE_BUS 3
  
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
MLX90614 mlx;

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  
  // Begin the wire
  Wire.begin();
  
  // Server initialisation
  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  IPAddress myIP(192,168,100,6);
  Ethernet.begin(mac,myIP);

  // Start Server
  server.begin();
}
 
 
void loop(void)
{
  size_t size;
  float t_baa;
  float t_bcf;
  float t_ds;

  // Get the temps
  sensors.begin();
  sensors.requestTemperatures();
  t_ds = sensors.getTempCByIndex(0);
  t_baa = mlx.getObjTemp(0x55);
  t_bcf = mlx.getObjTemp(0x5A);


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


