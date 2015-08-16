// This works for Yun and Uno
#include <i2cmaster.h>


void setup() {
  Serial.begin(9600);
  Serial.println("Setup...");

  i2c_init(); //Initialise the i2c bus

}

void loop() {
  // Register address to read - 0x06 for ambient, 0x07 for object
  Serial.println(readtemp(0x06));
  Serial.println(readtemp(0x07));
  delay(1000);
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

  //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
  double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)
  double tempData = 0x0000; // zero out the data
  int frac; // data past the decimal point

  // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  return ((tempData * tempFactor) - 0.01) - 273.15;
}
