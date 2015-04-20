
/*****************************************************************************************
 *
 *   Title:         I2C Device Scanner
 *   Author:        Matthew Dinsdale <mdinsdale159@gmail.com>
 *   Date:          Sunday, February 20th, 2012
 *   Availability:  e-mail, or with consent from LJMU FSEA Team.
 *   Compiled Size: 3858 bytes.
 *
 *****************************************************************************************
 * 
 *   The included library and detailed info of each command can be found at
 *   http://dsscircuits.com/articles/arduino-i2c-master-library.html
 * 
 *   Sketch was coded using Version 5 of the library.
 * 
 ******************************************************************************************/

#include <I2C.h>

void setup()
{
  Serial.begin(9600);    //Enable serial output at 115200Baud
  Serial.println("Initialising I2C Scanner...");
  Serial.println();

  PORTC = (1 << PORTC4) | (1 << PORTC5);
  // Pins: Standard: SDA:A4  SCL:A5
  //       Mega:     SDA:D20 SCL:D21

  // Start Read cycle.
  int x = 0;
  int y = 3;     // Run for 'y' times. Change if you feel more is needed.
  while(x < y){
    ++x;

    I2c.begin();   // Starts I2C Protocol.
    delay(1000);   // Wait 1 second to allow all devices to initialise.
    I2c.scan();    // Scans and prints found address. A delay is built into this function.
    I2c.end();     // Stops I2C data.
    Serial.println();
    if (x == y){   // Stops loop once run through 'Y' amount of times.
      break;
    }
  }
  // End of Read cycle.

  // End of program info.
  Serial.println();
  Serial.print("Scanner ran ");
  Serial.print(x); // Prints the ammount of times the device bus was read.
  Serial.println(" times and was able to find the above device names.");
  Serial.println("To restart, Power Cycle or Push Reset button.");
  // Tell user that the script has finished and how to re-run.
  Serial.println("Program complete");
  Serial.println();
}

void loop()
{
  // Not used.
}
