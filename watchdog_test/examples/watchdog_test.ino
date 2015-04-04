#include <UIPEthernet.h>

int pulsePin = 2;
int led = 4;
unsigned long lastHeartbeat = 0;
unsigned long lastUptimeReport = 0;

// Set up server on port 1000
EthernetServer server = EthernetServer(1000);
// Client object
EthernetClient client;
EthernetClient client2;

// Gateway IP
IPAddress gw_ip(192,168,100,1);
// This system IP
IPAddress system_ip(192,168,100,7);

// the setup routine runs once when you press reset:
void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600); 
  Serial.println("Arduino reset");
  pinMode(led, OUTPUT);

  // Server initialisation
  uint8_t mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

  Ethernet.begin(mac,system_ip);

    // Start Server
    server.begin();

  // Send an initial heartbeat.
  heartbeat("Initial heartbeat");
}

// the loop routine runs over and over again forever:
void loop() {

	size_t size;

	if (client = server.available()) {

		while((size = client.available()) > 0) {
			uint8_t* msg = (uint8_t*)malloc(size);
			size = client.read(msg,size);
			Serial.write(msg,size);
			free(msg);
		}
		client.print("Hello, World!");
		client.stop();
	}  

	unsigned long uptime = millis();
	if ((uptime - lastUptimeReport) >= 20000) {

         if (client.connect(gw_ip, 80)) {
         	Serial.println("connected");
	        // Make a HTTP request:
	        client.println("GET / HTTP/1.1");
	        // client.println("Host: www.google.com");
	        client.println("Connection: close");
	        client.println();
	        heartbeat('Client connected to ' + String(gw_ip));
	        client.stop();
	        
	        delay(1000);
	    }
	    lastUptimeReport = (uptime - (uptime % 10000));
    }
  // delay in between loops
  delay(500);
}

void heartbeat(String message) {
  // Sink current to drain charge from watchdog circuit
  pinMode(pulsePin, OUTPUT);
  digitalWrite(pulsePin, LOW);
  digitalWrite(led, HIGH);
  delay(300);
  // Return to high-Z
  pinMode(pulsePin, INPUT);
  digitalWrite(led, LOW);
  lastHeartbeat = millis();
  Serial.println(message);
}