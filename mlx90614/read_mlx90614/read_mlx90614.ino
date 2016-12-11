// read_mlx90614.ino

#include "i2cmaster.h"

byte MLX_one = 0x1D;
byte MLX_two = 0x2B;

void setup() {
  Serial.begin(9600);
  Serial.println("Setup...");
  // Initialise some stuff
  i2c_init();
  PORTC = (1 << PORTC4) | (1 << PORTC5);
}

void loop() {
    ReadTemp(MLX_one);
    delay(1000);
    ReadTemp(MLX_two);
    delay(1000);
}

/**
* Utility function to read temperature from MLX at address MLXAddress
*
*/
float ReadTemp(byte address) {
  int data_low = 0;
  int data_high = 0;
  int pec = 0;
  byte MLXAddress = address<<1;

  Serial.print("> [ReadTemp] Read temperature ");
  if (MLXAddress == 0) {
  	Serial.print("using MLX univeral address");
  } else {
  	Serial.print("using MLX address ");
  	Serial.print(address, HEX);
  };
  Serial.print(": ");

  i2c_start_wait(MLXAddress + I2C_WRITE);
  i2c_write(0x07);
  i2c_rep_start(MLXAddress + I2C_READ);
  data_low = i2c_readAck();
  data_high = i2c_readAck();
  pec = i2c_readNak();
  i2c_stop();

  float temp = 0x0000;                   
  temp = (float)(((data_high & 0x007F) << 8) + data_low);
  temp = (temp * 0.02) - 273.16;
  Serial.print(temp);
  Serial.println(" C");
  return temp;
}
