// remap_mlx90614.ino

#include "i2cmaster.h"

// New slave address
byte NewMLXAddr = 0x2B;
// Uncomment this if you want to change address back to default value (0x5A)
//byte NewMLXAddr = 0x5A;

void setup(){
  Serial.begin(9600);
  Serial.println("Setup...");
  // Initialise some stuff
  i2c_init();
  PORTC = (1 << PORTC4) | (1 << PORTC5);
  delay(5000);
  // Read on universal address (0)
  ReadAddr(0);
  // Change to new address NewMLXAddr
  ChangeAddr(NewMLXAddr, 0x00);
  // Read on universal address (0)
  ReadAddr(0);
  // Signal user to cycle power
  Serial.println("> [setup] Cycle power NOW to store new address - you have 10 seconds");
  delay(10000);
  // Read on universal address (0)
  ReadAddr(0);
  Serial.println("  Warning, next ReadTemp() may fail if address has not been set.");
  ReadTemp(NewMLXAddr);
  Serial.println("**---DONE---**");
}

/**
* Read temperature from MLX at new address once setup() is done.
*
*/
void loop(){
    delay(5000);
    ReadTemp(NewMLXAddr);
}

/**
* Changes the address of the MLX to NewAddr1.
*
*/
word ChangeAddr(byte NewAddr1, byte NewAddr2) {
  Serial.print("> [ChangeAddr] Will change address to: ");
  Serial.println(NewAddr1, HEX);
  i2c_start_wait(0 + I2C_WRITE);
  i2c_write(0x2E);
  i2c_write(0x00);
  i2c_write(0x00);

  if (i2c_write(0x6F) == 0) {
    i2c_stop();
    Serial.println("> [ChangeAddr] Data erased.");
  }
  else {
    i2c_stop();
    Serial.println("> [ChangeAddr] Failed to erase data");
    return -1;
  }

  Serial.print("> [ChangeAddr] Writing data: ");
  Serial.print(NewAddr1, HEX);
  Serial.print(", ");
  Serial.println(NewAddr2, HEX);

  for (int a = 0; a != 256; a++) {
    i2c_start_wait(0 + I2C_WRITE);
    i2c_write(0x2E);
    i2c_write(NewAddr1);
    i2c_write(NewAddr2);

    if (i2c_write(a) == 0) {
      i2c_stop();
      delay(100);
      Serial.print("> [ChangeAddr] Found correct CRC: 0x");
      Serial.println(a, HEX);
      return a;
    }
  }
  i2c_stop();
  Serial.println("> [ChangeAddr] Correct CRC not found");
  return -1;
}

/**
* Reads the MLX address.
*
*/
byte ReadAddr(byte MLXAddress) {
  byte NewMLXAddress;
  Serial.println("> [ReadAddr] Reading address");
  if (MLXAddress == 0) {
  	Serial.print("  Using MLX univeral address");
  } else {
  	Serial.print("  Using MLX address: ");
  	Serial.print(MLXAddress, HEX);
  }
  Serial.print(", Data: ");
  i2c_start_wait(MLXAddress + I2C_WRITE);
  i2c_write(0x2E);
  i2c_rep_start(MLXAddress + I2C_READ);
  
  NewMLXAddress = i2c_readAck();
  Serial.print(NewMLXAddress, HEX);             
  Serial.print(", ");
  Serial.print(i2c_readAck(), HEX);
  Serial.print(", ");
  Serial.println(i2c_readNak(), HEX);
  i2c_stop();
  return NewMLXAddress;
}
