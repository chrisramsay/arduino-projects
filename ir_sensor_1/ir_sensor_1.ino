/* Here is a bare-bones Arduino sketch to read the
   outputs of the photodiode detectors described
   in Figure 1 and Figure 2. The output pin of the
   circuit's opamp is connected to analog pin 0
   of the Arduino. The data are read every 0.1 second
   in this example. However, you can change the sampling
   rate by simply changing the argument of the
   delay() statement. */
 
#define inPin0 0
 
void setup(void) {
 
  Serial.begin(9600);
  Serial.println();
     
}
 
void loop(void) {
   
  int pinRead0 = analogRead(inPin0);
  float pVolt0 = pinRead0 / 1024.0 * 5.0;
  Serial.print(pVolt0);
  Serial.println();
   
  delay(100);
   
}
