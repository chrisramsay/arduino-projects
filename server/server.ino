#include <UIPEthernet.h>

// network configuration.  gateway and subnet are optional.

 // the media access control (ethernet hardware) address for the shield:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  
//the IP address for the shield:
byte ip[] = { 192, 168, 100, 8 };    
// the router's gateway address:
byte gateway[] = { 192, 168, 100, 1 };
// the subnet:
byte subnet[] = { 255, 255, 255, 0 };

// telnet defaults to port 1000
EthernetServer server = EthernetServer(1000);

void setup()
{
  Serial.begin(9600); 
  // initialize the ethernet device
  Ethernet.begin(mac, ip, gateway, subnet);
  Serial.begin(9600); 
  // start listening for clients
  server.begin();
}

void loop()
{
  size_t size;
  // if an incoming client connects, there will be bytes available to read:
  EthernetClient client = server.available();
  if (client == true) {
    Serial.println("Client is TRUE");
    while((size = client.available()) > 0) {
      uint8_t* msg = (uint8_t*)malloc(size);
      size = client.read(msg,size);
      Serial.write(msg,size);
      free(msg);
    }
    client.print("Hello, World!");
    client.stop();
  }
}