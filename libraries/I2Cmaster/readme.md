i2cmaster
========

Author:   Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury

Basic routines for communicating with I2C slave devices. This single master 
implementation is limited to one bus master on the I2C bus. 

This I2c library is implemented as a compact assembler software implementation of the I2C protocol 
which runs on any AVR (i2cmaster.S) and as a TWI hardware interface for all AVR with built-in TWI hardware (twimaster.c).
Since the API for these two implementations is exactly the same, an application can be linked either against the
software I2C implementation or the hardware I2C implementation.

Use 4.7k pull-up resistor on the SDA and SCL pin.
 
Adapt the SCL and SDA port and pin definitions and eventually the delay routine in the module 
i2cmaster.S to your target when using the software I2C implementation ! 
 
Adjust the  CPU clock frequence F_CPU in twimaster.c or in the Makfile when using the TWI hardware implementaion.
