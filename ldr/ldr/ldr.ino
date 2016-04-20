int ldr_pin = 2;
int ldr_val;

void setup()
{
Serial.begin(9600);
}

void loop()

{
int ldr_val = analogRead(ldr_pin);
Serial.println(ldr_val);
delay(1000);
}
