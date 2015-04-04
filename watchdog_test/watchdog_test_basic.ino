int pulsePin = 2;
int led = 4;
unsigned long lastHeartbeat = 0;
unsigned long lastUptimeReport = 0;

void setup() {
  Serial.begin(9600); 
  Serial.println("*** Arduino reset");
  pinMode(led, OUTPUT);

  // Send an initial heartbeat.
  heartbeat(" <3 Initial heartbeat");
}

void loop() {  
	unsigned long uptime = millis();
	if ((uptime - lastUptimeReport) >= 10000) {
    lastUptimeReport = (uptime - (uptime % 10000));
    heartbeat(" <3 Still alive!");
  }
  // delay in between loops
  delay(500);
}

/**
 * Utility function to handle heartbeat pulse generation
 * LED and a serial message
 */
void heartbeat(String message) {
  // Sink current to drain charge from C2
  pinMode(pulsePin, OUTPUT);
  digitalWrite(pulsePin, LOW);
  // LED on
  digitalWrite(led, HIGH);
  // Give enough time for C2 to discharge
  delay(300);
  // Return to high-Z
  pinMode(pulsePin, INPUT);
  // LED off
  digitalWrite(led, LOW);
  lastHeartbeat = millis();
  Serial.println(message);
}