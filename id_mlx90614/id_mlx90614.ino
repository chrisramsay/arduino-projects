// id_mlx90614.ino

/*

Uno: A4 (SDA) and A5 (SCL)
Yun: D2 (SDA) and D3 (SCL)

*/

#include "i2cmaster.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Setup...");/**/
  // Initialise the i2c bus, enable pullups and then wait
  i2c_init();
  PORTC = (1 << PORTC4) | (1 << PORTC5);
  delay(5000);
  // Read current address bytes
  ReadAddr(0);
  Serial.println("**---DONE---**");
}

void loop() {}

void ReadAddr(byte Address) {
  Serial.println("> Read address");
  // Inform the user
  Serial.print("  MLX address: ");
  Serial.print(Address, HEX);
  Serial.print(", Data: ");

  // Send start condition and write bit
  i2c_start_wait(Address + I2C_WRITE);
  // Send command for device to return address
  i2c_write(0x2E);
  i2c_rep_start(Address + I2C_READ);

  // Read 1 byte and then send ack (x2)
  Serial.print(i2c_readAck(), HEX);
  Serial.print(", ");
  Serial.print(i2c_readAck(), HEX);
  Serial.print(", ");
  Serial.println(i2c_readNak(), HEX);
  i2c_stop();
}
