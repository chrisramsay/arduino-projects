 #include <OneWire.h>
 #define ONE_WIRE_BUS 3

 OneWire  oneWire(ONE_WIRE_BUS);

 void setup() {
   Serial.begin(9600);
   discover();
 }

 void loop() {}

 void discover(void) {
   byte i;
   byte data[12];
   byte addr[8];

   Serial.print("Starting...\n\r");
   while(oneWire.search(addr)) {
     Serial.print("\n\r\n\rDevice has address:\n\r");
     for( i = 0; i < 8; i++) {
       Serial.print("0x");
       if (addr[i] < 16) {
         Serial.print('0');
       }
       Serial.print(addr[i], HEX);
       if (i < 7) {
         Serial.print(", ");
       }
     }
     if ( OneWire::crc8( addr, 7) != addr[7]) {
       Serial.print("CRC is not valid!\n\r");
       return;
     }
   }
   Serial.println();
   Serial.print("Done");
   oneWire.reset_search();
   return;
 }