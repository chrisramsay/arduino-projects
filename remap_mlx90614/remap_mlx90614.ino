// remap_mlx90614.ino

#include "i2cmaster.h"

// New slave address
//byte NewMLXAddr = 0x2B;
// Uncomment this if you want to change address back to default value (0x5A)
byte NewMLXAddr = 0x5A;

void setup(){
  Serial.begin(9600);
  Serial.println("Setup...");/**/
  // Initialise the i2c bus, enable pullups and then wait
  i2c_init();
  PORTC = (1 << PORTC4) | (1 << PORTC5);
  delay(5000);
  // Read current address bytes
  ReadAddr(0);
  // Change address to new value (NewMLXAddr);
  ChangeAddr(NewMLXAddr, 0x00);
  // Read address bytes
  ReadAddr(0);
  Serial.println("> Cycle power NOW - you have 10 seconds");
  // Cycle power to MLX during this 10 second pause
  delay(10000);
  // Read temperature using default address
  ReadTemp(0);
  // Read temperature using new address
  ReadTemp(NewMLXAddr<<1);
  Serial.println("**---DONE---**");
}

void loop(){
    delay(5000);
    ReadTemp(NewMLXAddr<<1);
}

word ChangeAddr(byte NewAddr1, byte NewAddr2) {
  Serial.println("> Change address");
  // Send start condition and write bit
  i2c_start_wait(0 + I2C_WRITE);
  // Send command for device to return address
  i2c_write(0x2E);
  // Send low byte zero to erase
  i2c_write(0x00);
  // Send high byte zero to erase
  i2c_write(0x00);

  if (i2c_write(0x6F) == 0) {
    // Release bus, end transaction
    i2c_stop();
    Serial.println("> Data erased.");
  }
  else {
    // Release bus, end transaction
    i2c_stop();
    Serial.println("> Failed to erase data");
    return -1;
  }

  Serial.print("> Writing data: ");
  Serial.print(NewAddr1, HEX);
  Serial.print(", ");
  Serial.println(NewAddr2, HEX);

  for (int a = 0; a != 256; a++) {
    // Send start condition and write bit
    i2c_start_wait(0 + I2C_WRITE);
    // Send command for device to return address
    i2c_write(0x2E);
    // Send low byte zero to erase
    i2c_write(NewAddr1);
    // Send high byte zero to erase
    i2c_write(NewAddr2);
    if (i2c_write(a) == 0) {
      // Release bus, end transaction then wait 10ms
      i2c_stop();
      delay(100);
      Serial.print("> Found correct CRC: 0x");
      Serial.println(a, HEX);
      return a;
    }
  }

  // Release bus, end transaction
  i2c_stop();
  Serial.println("> Correct CRC not found");
  return -1;
}

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

float ReadTemp(byte Address) {
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  Serial.println("> Read temperature");
  // Inform the user
  Serial.print("  MLX address: ");
  Serial.print(Address, HEX);
  Serial.print(", ");

  i2c_start_wait(Address + I2C_WRITE);
  // Address of temp bytes
  i2c_write(0x07);
  // Read - the famous repeat start
  i2c_rep_start(Address + I2C_READ);
  // Read 1 byte and then send ack (x2)
  data_low = i2c_readAck();
  data_high = i2c_readAck();
  pec = i2c_readNak();
  i2c_stop();

  // This converts high and low bytes together and processes the temperature
  // MSB is a error bit and is ignored for temperatures
  // Zero out the data
  float temp = 0x0000;                   
  // This masks off the error bit of the high byte, then moves it left 
  // 8 bits and adds the low byte.
  temp = (float)(((data_high & 0x007F) << 8) + data_low);
  temp = (temp * 0.02) - 273.16;
  Serial.print(temp);
  Serial.println(" C");
  return temp;
}

