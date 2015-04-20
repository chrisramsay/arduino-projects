OneWire 2.2 Library
===================

This is a mirror of http://www.pjrc.com/teensy/td_libs_OneWire.html

OneWire lets you access 1-wire devices made by Maxim/Dallas, such as temperature sensors and ibutton secure memory. For temperature sensors, the DallasTemperature library can be used with this library.

OneWire communicates with 1-wire devices. To act as a 1-wire device, use the OneWireSlave library.

Hardware Requirements
=====================

OneWire requires a single 4.7K pullup resistor, connected between the pin and +5 volts. Then just connect each 1-wire device to the pin and ground. Some 1-wire devices can also connect to power, or get their power from the signal wire. Please refer to the specifications for the 1-wire devices you are using.

Basic Usage
===========

OneWire myWire(pin)
Create the OneWire object, using a specific pin. Even though you can connect many 1 wire devices to the same pin, if you have a large number, smaller groups each on their own pin can help isolate wiring problems. You can create multiple OneWire objects, one for each pin.

myWire.search(addrArray)
Search for the next device. The addrArray is an 8 byte array. If a device is found, addrArray is filled with the device's address and true is returned. If no more devices are found, false is returned.

myWire.reset_search()
Begin a new search. The next use of search will begin at the first device.

myWire.reset()
Reset the 1-wire bus. Usually this is needed before communicating with any device.

myWire.select(addrArray)
Select a device based on its address. After a reset, this is needed to choose which device you will use, and then all communication will be with that device, until another reset.

myWire.skip()
Skip the device selection. This only works if you have a single device, but you can avoid searching and use this to immediatly access your device.

myWire.write(num);
Write a byte.

myWire.write(num, 1);
Write a byte, and leave power applied to the 1 wire bus.

myWire.read()
Read a byte.

myWire.crc8(dataArray, length)
Compute a CRC check on an array of data.