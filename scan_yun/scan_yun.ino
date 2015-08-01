#include <Console.h>
#include <i2cmaster.h>
float tt = 0;

void setup()
{
  Bridge.begin();
  Console.begin();

  while (!Console) {
    ; // wait for Console port to connect.
  }
  //tt = scan();
  Console.println("You're connected to the Console!!!!");
  //Console.println(tt);
  Console.println("Done");

}

void loop()
{
  // Not used.
}

float scan()
{
  int dev = 0x5A << 1;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  i2c_start_wait(dev + I2C_WRITE);
  // Send register address to read - 0x06 for ambient, 0x07 for object
  i2c_write(0x07);

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
  tempData = (tempData * tempFactor) - 0.01;

  return tempData - 273.15;
}


