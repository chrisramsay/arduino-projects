// helloworld.ino
#include <UIPEthernet.h> // Used for Ethernet

// **** ETHERNET SETTING ****

byte mac[] = { 0x54, 0x34, 0x41, 0x30, 0x30, 0x31 };                                       

byte ip[] = {192, 168, 100, 9 };                        

EthernetServer server(1000);

void setup() {

  Serial.begin(9600);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("IP Address: ");
  // Serial.println(ip);
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) 
  {  
    Serial.println("-> New Connection");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) 
        {
          client.println("<html><title>Hello World!</title><body><h3>Hello World!</h3></body>");
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      } 
    }

    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    client.stop();
    Serial.println("   Disconnected\n");
  }
}