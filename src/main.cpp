#include <Arduino.h>

byte masc;
int effectOrder = 1;
struct led 
{
  const int pin;
  byte state;
};

led led_1 = {12, 0b00000001};
led led_2 = {13, 0b00000010};
led led_3 = {14, 0b00000100};
led led_4 = {27, 0b00001000};
led led_5 = {16, 0b00010000};
led led_6 = {17, 0b00100000};
led led_7 = {18, 0b01000000};
led led_8 = {19, 0b10000000};

void mascType();

void setup()
{
  Serial.begin(115200);
  pinMode(led_1.pin, OUTPUT);
  pinMode(led_2.pin, OUTPUT);
  pinMode(led_3.pin, OUTPUT);
  pinMode(led_4.pin, OUTPUT);
  pinMode(led_5.pin, OUTPUT);
  pinMode(led_6.pin, OUTPUT);
  pinMode(led_7.pin, OUTPUT);
  pinMode(led_8.pin, OUTPUT);
}

void loop()
{
  masc = 0b00000001;
  for(;;)
  {
    if(masc == 0)
    {
      effectOrder += 1;
      if (effectOrder > 4) effectOrder = 1;
      mascType();
    }
    digitalWrite(led_1.pin,masc & led_1.state);
    digitalWrite(led_2.pin,masc & led_2.state);
    digitalWrite(led_3.pin,masc & led_3.state);
    digitalWrite(led_4.pin,masc & led_4.state);
    digitalWrite(led_5.pin,masc & led_5.state);
    digitalWrite(led_6.pin,masc & led_6.state);
    digitalWrite(led_7.pin,masc & led_7.state);
    digitalWrite(led_8.pin,masc & led_8.state);

    masc *= 2;
    delay(200);
  }
}

void mascType()
{
  switch(effectOrder)
  {
    case 1:
      masc = 0b00000001;
      break;
    case 2:
      masc = 0b00010001;
      break;
    case 3:
      masc = 0b01010101;
      break;
    case 4:
      masc = 0b00000101;
      break;
  }
}
