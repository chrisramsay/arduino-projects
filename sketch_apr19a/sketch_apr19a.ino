#include <i2cmaster.h>
// Pins: Standard: SDA:A4  SCL:A5
//       Mega:     SDA:D20 SCL:D21

byte MLXAddr = 0x5A<<1;           // Default address
//byte MLXAddr = 0;               // Universal address

void setup(){
  Serial.begin(9600);
  Serial.println("Setup...");
  
  i2c_init();                              //Initialise the i2c bus
  PORTC = (1 << PORTC4) | (1 << PORTC5);   //enable pullups
}

void loop(){
    delay(1000); // wait a second
    ReadTemp(MLXAddr);              // Read temperature using new address
}

float ReadTemp(byte Address) {
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  Serial.println("> Read temperature");

  Serial.print("  MLX address: ");
  Serial.print(Address, HEX);
  Serial.print(", ");

  i2c_start_wait(Address + I2C_WRITE);
  i2c_write(0x07);                  // Address of temp bytes
  
  // read
  i2c_rep_start(Address + I2C_READ);
  data_low = i2c_readAck();         //Read 1 byte and then send ack
  data_high = i2c_readAck();        //Read 1 byte and then send ack
  pec = i2c_readNak();
  i2c_stop();
  
  //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
  float Temperature = 0x0000;       // zero out the data
  
  // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
  Temperature = (float)(((data_high & 0x007F) << 8) + data_low);
  Temperature = (Temperature * 0.02) - 273.16;
  
  Serial.print(Temperature);
  Serial.println(" C");
  return Temperature;
}
