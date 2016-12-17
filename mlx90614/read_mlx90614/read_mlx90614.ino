// read_mlx90614.ino

#include "i2cmaster.h"

byte MLX_one = 0x1D;
byte MLX_two = 0x2B;
//byte MLX_n = ;

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
    //ReadTemp(MLX_n);
    //delay(1000);
}

