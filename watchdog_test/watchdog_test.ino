int pulsePin = 8;
int led = 13;
unsigned long lastHeartbeat = 0;
unsigned long lastUptimeReport = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);  
  Serial.println("Arduino reset");
  pinMode(led, OUTPUT); 
  // Send an initial heartbeat.
  heartbeat("Initial heartbeat");
}

// the loop routine runs over and over again forever:
void loop() {
  // Check for serial inputs.  If found, send heartbeat.
  //if (Serial.available()) {
    // Clear input buffer
    //while (Serial.available()) {
    //  Serial.read();
    //}
    heartbeat("Heartbeat sent");
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
	delay(1000);               // wait for a second
	digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
	delay(1000); 
  //}
  unsigned long uptime = millis();
  if ((uptime - lastUptimeReport) >= 10000) {
    // It has been at least 5 seconds since our last uptime report.  
    Serial.println("Uptime: " + String((uptime - (uptime % 10000)) / 1000) + " seconds (" + String((uptime - lastHeartbeat) / 1000) + " seconds since last heartbeat)");
    // Pretend we did it exactly on the 5 second mark so we don't start slipping.
    lastUptimeReport = (uptime - (uptime % 10000));
  }
  // delay in between loops
  delay(100);
}

void heartbeat(String message) {
  // Sink current to drain charge from watchdog circuit
  pinMode(pulsePin, OUTPUT);
  digitalWrite(pulsePin, LOW);
  delay(300);
  // Return to high-Z
  pinMode(pulsePin, INPUT);
  lastHeartbeat = millis();
  Serial.println(message);
}