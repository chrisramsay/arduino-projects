#include <i2cmaster.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

void setup() {
  Serial.begin(9600);
  i2c_init(); //Initialise the i2c bus
  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
}

void loop() {
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');

  // is "temp" command?
  // Example: http://arduino.local/temp/
  if (command == "temp") {
    tempCommand(client);
  }
}

void tempCommand(YunClient client) {
  // Read the command
  String mode = client.readStringUntil('\r');
  // Switch on mode...
  // Example: http://arduino.local/temp/amb
  if (mode == "amb") {
    client.print(F("Ambient: "));
    client.print(readtemp(0x06));
    client.println();
  }
  // Example: http://arduino.local/temp/obj
  if (mode == "obj") {
    client.print(F("Object: "));
    client.print(readtemp(0x07));
    client.println();
  }
  // Example: http://arduino.local/temp/all
  if (mode == "all") {
    client.print(F("Ambient: "));
    client.print(readtemp(0x06));
    client.println();
    client.print(F("Object: "));
    client.print(readtemp(0x07));
    client.println();
  }
  // Example: http://arduino.local/temp/csv
  if (mode == "csv") {
    client.print(readtemp(0x06));
    client.print(",");
    client.print(readtemp(0x07));
    client.print(",");
    client.println();
  }
}

float readtemp(int sensor) {

  int dev = 0x5A << 1;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  i2c_start_wait(dev + I2C_WRITE);

  i2c_write(sensor);

  // read
  i2c_rep_start(dev + I2C_READ);
  data_low = i2c_readAck(); //Read 1 byte and then send ack
  data_high = i2c_readAck(); //Read 1 byte and then send ack
  pec = i2c_readNak();
  i2c_stop();

  // This converts high and low bytes together and processes temperature,
  // MSB is a error bit and is ignored for temps
  // 0.02 degrees per LSB (measurement resolution of the MLX90614)
  double tempFactor = 0.02;
  // zero out the data
  double tempData = 0x0000;
  // data past the decimal point
  int frac;

  // This masks off the error bit of the high byte,
  // then moves it left 8 bits and adds the low byte.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  return ((tempData * tempFactor) - 0.01) - 273.15;
}

