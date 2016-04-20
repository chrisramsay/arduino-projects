// constants won't change. Used here to set a pin number :
const int laserPin =  12;      // the number of the Laser pin

// Variables will change :
int laserState = LOW;             // laserState used to set the Laser

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time Laser was updated

// constants won't change :
const long interval = 1000;           // interval at which to blink (milliseconds)
const long measure = 4000;           // interval at which to blink (milliseconds)

int LaserMax = 0;

void setup() {
  // set the digital pin as output:
  pinMode(laserPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // read the input on analog pin 2:
  int LDRValue = analogRead(A2);
  // print out the value you read:
  if (LDRValue >= LaserMax) {
    LaserMax = LDRValue;
  }
  Serial.println(LaserMax);
  delay(50);
  
  // check to see if it's time to blink the Laser; that is, if the
  // difference between the current time and last time you blinked
  // the Laser is bigger than the interval at which you want to
  // blink the Laser.
  unsigned long currentMillis = millis();

  //if (currentMillis - previousMillis >= measure) {
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the Laser
    previousMillis = currentMillis;

    // if the Laser is off turn it on and vice-versa:
    if (laserState == LOW) {
      laserState = HIGH;
    } else {
      laserState = LOW;
    }


    // save the last time you blinked the Laser
    //previousMillis = currentMillis;
    //LaserMax = 0;
    //Serial.println("LOW ---");
  //}
    
    // set the Laser with the laserState of the variable:
    digitalWrite(laserPin, laserState);
  }
}

